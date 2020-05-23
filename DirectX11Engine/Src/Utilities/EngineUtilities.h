#pragma once
#include <string>

namespace Utility
{
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
}
