#pragma once
#include "Bindable.h"
#include <unordered_map>
class BindableCodex
{
public:
	template<class T, typename...Params>
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx, Params&&...p) noexcept
	{
		return Get().Resolve_<T>(gfx, std::forward<Params>(p)...);
	}
	static BindableCodex& Get()
	{
		static BindableCodex codex;
		return codex;
	}
private:
	template<class T, typename...Params>
	std::shared_ptr<Bindable> Resolve_(Graphics& gfx, Params&& ...p) noexcept
	{
		const auto key = T::GenerateUID(std::forward<Params>(p)...);
		const auto i = bindMap.find(key);

		if (i == bindMap.end())
		{
			auto bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);
			bindMap[key] = bind;
			return bind;
		}
		else
		{
			return i->second;
		}
	}
	std::unordered_map<std::string, std::shared_ptr<Bindable>> bindMap;
};