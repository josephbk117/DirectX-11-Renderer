#include "Rasterizer.h"
#include "BindableCodex.h"

Rasterizer::Rasterizer(Graphics& gfx, bool twoSided) : isTwoSided(twoSided)
{
	D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	//rasterDesc.CullMode = D3D11_CULL_NONE;
	//rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	//rasterDesc.AntialiasedLineEnable = TRUE;
	gfx.GetDevice()->CreateRasterizerState(&rasterDesc, &pRasterizer);
}

void Rasterizer::Bind(Graphics& gfx) noexcept
{
	gfx.GetContext()->RSSetState(pRasterizer.Get());
}

std::shared_ptr<Bindable> Rasterizer::Resolve(Graphics& gfx, bool twoSided)
{
	return BindableCodex::Resolve<Rasterizer>(gfx, twoSided);
}

std::string Rasterizer::GenerateUID(bool twoSided)
{
	using namespace std::string_literals;
	return typeid(Rasterizer).name() + "#"s + (twoSided ? "y" : "n");
}

std::string Rasterizer::GetUID() const noexcept
{
	return GenerateUID(isTwoSided);
}