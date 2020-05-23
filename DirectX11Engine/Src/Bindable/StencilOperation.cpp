#include "StencilOperation.h"
#include "BindableCodex.h"

StencilOperation::StencilOperation(Graphics& gfx, Mode mode) : mode(mode)
{
	D3D11_DEPTH_STENCIL_DESC desc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());

	if (mode == Mode::Write)
	{
		desc.StencilEnable = TRUE;
		desc.StencilWriteMask = 0xFF;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	}
	else if (mode == Mode::Mask)
	{
		desc.DepthEnable = FALSE;
		desc.StencilEnable = FALSE;
		desc.StencilReadMask = 0xFF;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	}

	gfx.GetDevice()->CreateDepthStencilState(&desc, &pDepthStencil);
}

void StencilOperation::Bind(Graphics& gfx) noexcept
{
	gfx.GetContext()->OMSetDepthStencilState(pDepthStencil.Get(), 0xFF);
}

std::shared_ptr<Bindable> StencilOperation::Resolve(Graphics& gfx, Mode mode)
{
	return BindableCodex::Resolve<StencilOperation>(gfx, mode);
}

std::string StencilOperation::GenerateUID(Mode mode)
{
	using namespace std::string_literals;
	return typeid(StencilOperation).name() + "#"s + std::to_string((int)mode);
}

std::string StencilOperation::GetUID() const noexcept
{
	return GenerateUID(mode);
}
