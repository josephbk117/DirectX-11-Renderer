#include "BlendOperation.h"
#include "BindableCodex.h"

BlendOperation::BlendOperation(Graphics& gfx, bool blendingEnabled) : isBlendingEnabled(blendingEnabled)
{
	D3D11_BLEND_DESC blendDesc = {};
	D3D11_RENDER_TARGET_BLEND_DESC& brt = blendDesc.RenderTarget[0];

	if (isBlendingEnabled)
	{
		brt.BlendEnable = TRUE;
		brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		brt.BlendOp = D3D11_BLEND_OP_ADD;
		brt.SrcBlendAlpha = D3D11_BLEND_ONE;
		brt.DestBlendAlpha = D3D11_BLEND_ONE;
		brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	else
	{
		brt.BlendEnable = FALSE;
		brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	GetDevice(gfx)->CreateBlendState(&blendDesc, &pBlendOp);
}

void BlendOperation::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->OMSetBlendState(pBlendOp.Get(), nullptr, 0xFFFFFFFF);
}

std::shared_ptr<Bindable> BlendOperation::Resolve(Graphics& gfx, bool blendingEnabled)
{
	return BindableCodex::Resolve<BlendOperation>(gfx, blendingEnabled);
}

std::string BlendOperation::GenerateUID(bool blendingEnabled)
{
	using namespace std::string_literals;
	return typeid(BlendOperation).name() + "#"s + (blendingEnabled ? "y" : "n");
}

std::string BlendOperation::GetUID() const noexcept
{
	return GenerateUID(isBlendingEnabled);
}
