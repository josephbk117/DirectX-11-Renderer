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
	HRESULT hr = DirectX::LoadFromWICFile(Utility::StringToWString(imagePath).c_str(), DirectX::WIC_FLAGS_FORCE_RGB, nullptr, scratch);

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

