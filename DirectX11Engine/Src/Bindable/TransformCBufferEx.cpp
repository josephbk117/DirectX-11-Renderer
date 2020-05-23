#include "TransformCBufferEx.h"

TransformCBufferEx::TransformCBufferEx(Graphics& gfx, const Drawable& parent, UINT slotVertex /*= 0*/, UINT slotPixel /*=0*/) noexcept
	:TransformCbuffer(gfx, parent, slotVertex)
{
	if (!pcBuf)
	{
		pcBuf = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, slotPixel);
	}
}

void TransformCBufferEx::Bind(Graphics& gfx) noexcept
{
	const auto tf = GetTransforms(gfx);
	TransformCbuffer::UpdateBindImpl(gfx, tf);
	UpdateBindImpl(gfx, tf);
}

void TransformCBufferEx::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
{
	pcBuf->Update(gfx, tf);
	pcBuf->Bind(gfx);
}

std::unique_ptr<PixelConstantBuffer<TransformCbuffer::Transforms>> TransformCBufferEx::pcBuf;