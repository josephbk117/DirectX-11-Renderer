#pragma once
#include "ConstantBuffers.h"
#include "..\Drawable\Drawable.h"
#include <DirectXMath.h>

class TransformCbuffer : public Bindable
{
public:
	TransformCbuffer(Graphics& gfx, const Drawable& parent, UINT slot = 0);
	void Bind(Graphics& gfx) noexcept override;
protected:
	struct Transforms
	{
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX modelView;
		DirectX::XMMATRIX modelViewProj;
		DirectX::XMVECTOR camPos;
	};
	void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;
	Transforms GetTransforms(Graphics& gfx) noexcept;
private:
	static std::unique_ptr<VertexConstantBuffer<Transforms>> vcbuf;
	const Drawable& parent;
};