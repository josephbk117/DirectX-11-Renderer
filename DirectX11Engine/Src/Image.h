#pragma once
#include "GlobalDefines.h"
#include "DirectXTex/DirectXTex.h"
#include <vector>

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

