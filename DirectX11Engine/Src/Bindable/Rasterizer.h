#pragma once
#include "Bindable.h"
class Rasterizer : public Bindable
{
public:
	Rasterizer(Graphics& gfx, bool twoSided);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx, bool twoSided);
	static std::string GenerateUID(bool twoSided);
	std::string GetUID() const noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
	bool isTwoSided = false;
};

