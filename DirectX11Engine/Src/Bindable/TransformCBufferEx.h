#pragma once
#include "TransformCBuffer.h"
class TransformCBufferEx : public TransformCbuffer
{
public:
	TransformCBufferEx(Graphics& gfx, const Drawable& parent, UINT slotVertex = 0,  UINT slotPixel = 0)noexcept;
	void Bind(Graphics& gfx) noexcept override;
protected:
	void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;
private:
	static std::unique_ptr<PixelConstantBuffer<Transforms>> pcBuf;
};

