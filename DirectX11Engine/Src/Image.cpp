#include "Image.h"
#include "GlobalDefines.h"
#include "Utilities/EngineUtilities.h"
#include <assert.h>


Image::Image(int width, int height) noexcept
{
	HRESULT hr = scratch.Initialize2D(format, width, height, 1u, 1u);
	if (FAILED(hr))
	{
		assert("Failed to initialize image");
	}
}

Image Image::FromFile(const std::string& imagePath)
{
	DirectX::ScratchImage scratch;
	HRESULT hr = DirectX::LoadFromWICFile(Utility::StringToWString(imagePath).c_str(), 
		DirectX::WIC_FLAGS_FORCE_RGB | DirectX::WIC_FLAGS_FORCE_LINEAR | DirectX::WIC_FLAGS_FILTER_LINEAR, nullptr, scratch);

	if (FAILED(hr))
	{
		assert("Failed to load image");
	}

	return Image(std::move(scratch), imagePath);
}

Image::Image(DirectX::ScratchImage scratch, const std::string& path) noexcept : scratch(std::move(scratch)) {}

int Image::GetWidth() const noexcept
{
	return (unsigned int)scratch.GetMetadata().width;
}

int Image::GetHeight() const noexcept
{
	return (unsigned int)scratch.GetMetadata().height;
}

Image::Color* Image::GetData() const noexcept
{
	return reinterpret_cast<Color*>(scratch.GetPixels());
}

void Image::ClearData(Color clearValue) noexcept
{
	const auto width = GetWidth();
	const auto height = GetHeight();
	auto& imgData = *scratch.GetImage(0, 0, 0);
	for (size_t y = 0u; y < height; y++)
	{
		auto rowStart = reinterpret_cast<Color*>(imgData.pixels + imgData.rowPitch * y);
		std::fill(rowStart, rowStart + imgData.width, clearValue);
	}
}

int Image::GetPixelCount() const noexcept
{
	return GetWidth() * GetHeight();
}

int Image::GetAllocatedMemorySize() const noexcept
{
	return GetPixelCount() * sizeof(Color);
}

//-----------------------IMAGE HDR-------------------------//

ImageHDR::ImageHDR(int width, int height) noexcept
{
	HRESULT hr = scratch.Initialize2D(format, width, height, 1u, 1u);
	if (FAILED(hr))
	{
		assert("Failed to initialize image");
	}
}

ImageHDR ImageHDR::FromFile(const std::string& name)
{
	DirectX::ScratchImage scratch;
	HRESULT hr = DirectX::LoadFromWICFile(Utility::StringToWString(name).c_str(), DirectX::WIC_FLAGS_FORCE_RGB, nullptr, scratch);

	if (FAILED(hr))
	{
		assert("Failed to load image");
	}

	return ImageHDR(std::move(scratch), name);
}


ImageHDR ImageHDR::FromData(std::vector<ColorFloat>& colourData, unsigned int width, unsigned int height)
{
	DirectX::ScratchImage scratch;
	DirectX::Image img;

	const unsigned int COLOR_FLOAT_SIZE = sizeof(ColorFloat);

	img.format = format;
	img.height = height;
	img.width = width;
	img.rowPitch = img.width * COLOR_FLOAT_SIZE;
	img.slicePitch = 0;
	img.pixels = new uint8_t[img.width * img.height * COLOR_FLOAT_SIZE];

	const unsigned int ITERATION_COUNT = img.height * img.width * COLOR_FLOAT_SIZE;

	for (int i = 0; i < ITERATION_COUNT; i+= COLOR_FLOAT_SIZE)
	{
		uint8_t* pp = reinterpret_cast<uint8_t*>(colourData.data() + i/ COLOR_FLOAT_SIZE);
		memcpy(img.pixels + i, pp, COLOR_FLOAT_SIZE);
	}

	HRESULT hr = scratch.InitializeFromImage(img);

	delete[] img.pixels;

	if (FAILED(hr))
	{
		assert("Failed to create image");
	}

	return ImageHDR(std::move(scratch), "null");
}

ImageHDR::ImageHDR(DirectX::ScratchImage scratch, const std::string& path) noexcept : scratch(std::move(scratch)) {}


int ImageHDR::GetWidth() const noexcept
{
	return (unsigned int)scratch.GetMetadata().width;
}

int ImageHDR::GetHeight() const noexcept
{
	return (unsigned int)scratch.GetMetadata().height;
}

ImageHDR::ColorFloat* ImageHDR::GetData() const noexcept
{
	return reinterpret_cast<ColorFloat*>(scratch.GetPixels());
}

void ImageHDR::ClearData(ColorFloat clearValue) noexcept
{
	const auto width = GetWidth();
	const auto height = GetHeight();
	auto& imgData = *scratch.GetImage(0, 0, 0);
	for (size_t y = 0u; y < height; y++)
	{
		auto rowStart = reinterpret_cast<ColorFloat*>(imgData.pixels + imgData.rowPitch * y);
		std::fill(rowStart, rowStart + imgData.width, clearValue);
	}
}

int ImageHDR::GetPixelCount() const noexcept
{
	return GetWidth() * GetHeight();
}

int ImageHDR::GetAllocatedMemorySize() const noexcept
{
	return GetPixelCount() * sizeof(ColorFloat);
}