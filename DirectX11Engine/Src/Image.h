#pragma once
#include "GlobalDefines.h"
#include "DirectXTex/DirectXTex.h"
#include <vector>
#include <string>

class Image
{
public:
	class Color
	{
	public:
		unsigned int dword;
	public:
		constexpr Color() noexcept : dword()
		{}
		constexpr Color(const Color& col) noexcept
			:
			dword(col.dword)
		{}
		constexpr Color(unsigned int dw) noexcept
			:
			dword(dw)
		{}
		constexpr Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b) noexcept
			:
			dword((x << 24u) | (b << 16u) | (g << 8u) | r)
		{}
		constexpr Color(unsigned char r, unsigned char g, unsigned char b) noexcept
			:
			dword((255u << 24u) | (b << 16u) | (g << 8u) | r)
		{}
		constexpr Color(Color col, unsigned char x) noexcept
			:
			Color((x << 24u) | col.dword)
		{}
		Color& operator =(Color color) noexcept
		{
			dword = color.dword;
			return *this;
		}
		constexpr unsigned char GetX() const noexcept
		{
			return dword >> 24u;
		}
		constexpr unsigned char GetA() const noexcept
		{
			return GetX();
		}
		constexpr unsigned char GetR() const noexcept
		{
			return (dword >> 16u) & 0xFFu;
		}
		constexpr unsigned char GetG() const noexcept
		{
			return (dword >> 8u) & 0xFFu;
		}
		constexpr unsigned char GetB() const noexcept
		{
			return dword & 0xFFu;
		}
		void SetX(unsigned char x) noexcept
		{
			dword = (dword & 0xFFFFFFu) | (x << 24u);
		}
		void SetA(unsigned char a) noexcept
		{
			SetX(a);
		}
		void SetR(unsigned char r) noexcept
		{
			dword = (dword & 0xFF00FFFFu) | (r << 16u);
		}
		void SetG(unsigned char g) noexcept
		{
			dword = (dword & 0xFFFF00FFu) | (g << 8u);
		}
		void SetB(unsigned char b) noexcept
		{
			dword = (dword & 0xFFFFFF00u) | b;
		}
	};

public:
	Image() noexcept = default;
	Image(int width, int height) noexcept;

	static Image FromFile(const std::string& name);

	Image(Image&& source) noexcept = default;
	Image(Image&) = delete;
	Image& operator=(Image&& donor) noexcept = default;
	Image& operator=(const Image&) = delete;

	int GetWidth() const noexcept;
	int GetHeight() const noexcept;
	Color* GetData() const noexcept;
	void ClearData(Color clearValue) noexcept;
	int GetPixelCount() const noexcept;
	int GetAllocatedMemorySize() const noexcept;
	~Image() = default;
private:
	Image(DirectX::ScratchImage scratch, const std::string& path) noexcept;
	static constexpr DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	DirectX::ScratchImage scratch;
};

class ImageHDR
{
public:
	class ColorFloat
	{
	public:
		DirectX::XMFLOAT4 data{ 0 };
	public:
		constexpr ColorFloat() noexcept : data()
		{}
		constexpr ColorFloat(const ColorFloat& col) noexcept
			:
			data(col.data)
		{}
		constexpr ColorFloat(float d) noexcept
			:
			data(d, d, d, d)
		{}
		constexpr ColorFloat(float r, float g, float b, float a) noexcept
			:
			data(r, g, b, a)
		{}
		constexpr ColorFloat(float r, float g, float b) noexcept
			:
			data(r, g, b, 1.0f)
		{}
		ColorFloat& operator =(ColorFloat color) noexcept
		{
			data = color.data;
			return *this;
		}
		constexpr float GetA() const noexcept
		{
			return data.z;
		}
		constexpr float GetR() const noexcept
		{
			return data.w;
		}
		constexpr float GetG() const noexcept
		{
			return data.x;
		}
		constexpr float GetB() const noexcept
		{
			return data.y;
		}
		void SetA(float a) noexcept
		{
			data.w = a;
		}
		void SetR(float r) noexcept
		{
			data.x = r;
		}
		void SetG(float g) noexcept
		{
			data.y = g;
		}
		void SetB(float b) noexcept
		{
			data.z = b;
		}

		ColorFloat operator+(const ColorFloat& col) noexcept
		{
			ColorFloat outCol;

			outCol.data.w = this->data.w + col.data.w;
			outCol.data.x = this->data.x + col.data.x;
			outCol.data.y = this->data.y + col.data.y;
			outCol.data.z = this->data.z + col.data.z;

			return outCol;
		}

		ColorFloat operator-(const ColorFloat& col) noexcept
		{
			ColorFloat outCol;

			outCol.data.w = this->data.w - col.data.w;
			outCol.data.x = this->data.x - col.data.x;
			outCol.data.y = this->data.y - col.data.y;
			outCol.data.z = this->data.z - col.data.z;

			return outCol;
		}

		ColorFloat operator*(const ColorFloat& col) noexcept
		{
			ColorFloat outCol;

			outCol.data.w = this->data.w * col.data.w;
			outCol.data.x = this->data.x * col.data.x;
			outCol.data.y = this->data.y * col.data.y;
			outCol.data.z = this->data.z * col.data.z;

			return outCol;
		}

		ColorFloat operator*(float scalar) noexcept
		{
			ColorFloat outCol;

			outCol.data.w = this->data.w * scalar;
			outCol.data.x = this->data.x * scalar;
			outCol.data.y = this->data.y * scalar;
			outCol.data.z = this->data.z * scalar;

			return outCol;
		}

		ColorFloat operator/(const ColorFloat& col) noexcept
		{
			ColorFloat outCol;

			outCol.data.w = this->data.w / col.data.w;
			outCol.data.x = this->data.x / col.data.x;
			outCol.data.y = this->data.y / col.data.y;
			outCol.data.z = this->data.z / col.data.z;

			return outCol;
		}

		ColorFloat operator/(float scalar) noexcept
		{
			ColorFloat outCol;

			outCol.data.x = this->data.x / scalar;
			outCol.data.y = this->data.y / scalar;
			outCol.data.w = this->data.w / scalar;
			outCol.data.z = this->data.z / scalar;

			return outCol;
		}
	};

public:
	static constexpr DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;

	ImageHDR() noexcept = default;
	ImageHDR(int width, int height) noexcept;

	static ImageHDR FromFile(const std::string& name);
	static ImageHDR FromData(std::vector<ColorFloat>& colourData, unsigned int width, unsigned int height);

	ImageHDR(ImageHDR&& source) noexcept = default;
	ImageHDR(ImageHDR&) = delete;
	ImageHDR& operator=(ImageHDR&& donor) noexcept = default;
	ImageHDR& operator=(const ImageHDR&) = delete;

	int GetWidth() const noexcept;
	int GetHeight() const noexcept;
	ColorFloat* GetData() const noexcept;
	void ClearData(ColorFloat clearValue) noexcept;
	int GetPixelCount() const noexcept;
	int GetAllocatedMemorySize() const noexcept;
	~ImageHDR() = default;
private:
	ImageHDR(DirectX::ScratchImage scratch, const std::string& path) noexcept;
	DirectX::ScratchImage scratch;
};

