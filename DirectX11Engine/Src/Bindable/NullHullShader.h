#pragma once
#include "Bindable.h"
class NullHullShader : public Bindable
{
public:
	NullHullShader(Graphics& gfx);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx);
	static std::string GenerateUID();
	std::string GetUID() const noexcept override;
};
