#pragma once
#include <string>
#include <DirectXMath.h>

namespace Utility
{
	using namespace DirectX;

	static std::wstring StringToWString(const std::string& s)
	{
		using namespace std::string_literals;
		std::wstring temp(s.length(), L' ');
		std::copy(s.begin(), s.end(), temp.begin());
		return temp;
	}

	static std::string WStringToString(const std::wstring& s)
	{
		std::string temp(s.length(), ' ');
		std::copy(s.begin(), s.end(), temp.begin());
		return temp;
	}

	static XMFLOAT3 AddFloat3(XMFLOAT3 a, XMFLOAT3 b)
	{
		XMVECTOR add = XMVectorAdd(XMLoadFloat3(&a), XMLoadFloat3(&b));
		XMFLOAT3 out;
		XMStoreFloat3(&out, add);
		return out;
	}

	template<typename T>
	static int FindIndexOf(std::vector<T> v, T x)
	{
		return std::find(v.begin(), v.end(), x) - v.begin();
	}

	template<typename T>
	static bool Contains(std::vector<T> v, T x)
	{
		return std::find(v.begin(), v.end(), x) == v.end();
	}
}
