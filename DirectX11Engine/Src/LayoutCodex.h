#pragma once
#include "DynamicConstantBuffer.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace Dcb
{
	class LayoutCodex
	{
	public:
		static Dcb::CookedLayout Resolve(Dcb::RawLayout&& layout);
	private:
		static LayoutCodex& Get_() noexcept;
		std::unordered_map<std::string, std::shared_ptr<Dcb::LayoutElement>> map;
	};
}