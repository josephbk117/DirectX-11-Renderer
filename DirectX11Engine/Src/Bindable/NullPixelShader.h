#pragma once
#include "Bindable.h"
class NullPixelShader : public Bindable
{
public:
	NullPixelShader(Graphics& gfx);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx);
	static std::string GenerateUID();
	std::string GetUID() const noexcept override;
};

