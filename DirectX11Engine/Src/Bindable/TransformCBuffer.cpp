#include "TransformCBuffer.h"
TransformCbuffer::TransformCbuffer(Graphics& gfx, const Drawable& parent, UINT slot) : parent(parent)
{
	if (!vcbuf)
	{
		vcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
	}
}

void TransformCbuffer::Bind(Graphics& gfx) noexcept
{
	UpdateBindImpl(gfx, GetTransforms(gfx));
}

void TransformCbuffer::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
{
	vcbuf->Update(gfx, tf);
	vcbuf->Bind(gfx);
}

TransformCbuffer::Transforms TransformCbuffer::GetTransforms(Graphics& gfx) noexcept
{
	const auto modelView = parent.GetTransformXM() * gfx.GetCamera();
	return {
		DirectX::XMMatrixTranspose(parent.GetTransformXM()),
		DirectX::XMMatrixTranspose(gfx.GetCamera()),
		DirectX::XMMatrixTranspose(gfx.GetProjection()),
		DirectX::XMMatrixTranspose(modelView),
		DirectX::XMMatrixTranspose(modelView * gfx.GetProjection()),
		gfx.GetCameraPos()
	};
}

std::unique_ptr<VertexConstantBuffer<TransformCbuffer::Transforms>> TransformCbuffer::vcbuf;