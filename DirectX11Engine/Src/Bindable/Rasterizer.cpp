#include "Rasterizer.h"
#include "BindableCodex.h"

Rasterizer::Rasterizer(Graphics& gfx, RasterizerMode rasterizerMode) : rasterizerMode(rasterizerMode)
{
	D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rasterDesc.CullMode = rasterizerMode.isTwoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;
	rasterDesc.FillMode = rasterizerMode.isWireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	GetDevice(gfx)->CreateRasterizerState(&rasterDesc, &pRasterizer);
}

void Rasterizer::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->RSSetState(pRasterizer.Get());
}

std::shared_ptr<Bindable> Rasterizer::Resolve(Graphics& gfx, RasterizerMode rasterizerMode)
{
	return BindableCodex::Resolve<Rasterizer>(gfx, rasterizerMode);
}

std::string Rasterizer::GenerateUID(RasterizerMode rasterizerMode)
{
	using namespace std::string_literals;
	return typeid(Rasterizer).name() + "#"s + (rasterizerMode.isTwoSided ?"ts y" : " ts n" + (rasterizerMode.isWireframe) ? "wf y" : "wf n");
}

std::string Rasterizer::GetUID() const noexcept
{
	return GenerateUID(rasterizerMode);
}