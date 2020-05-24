#pragma once
#include "Bindable.h"
class Rasterizer : public Bindable
{
public:

	struct RasterizerMode
	{
		bool isTwoSided = false;
		bool isWireframe = false;
	};

	Rasterizer(Graphics& gfx, RasterizerMode rasterizerMode);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx, RasterizerMode rasterizerMode);
	static std::string GenerateUID(RasterizerMode rasterizerMode);
	std::string GetUID() const noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
	RasterizerMode rasterizerMode;
};

