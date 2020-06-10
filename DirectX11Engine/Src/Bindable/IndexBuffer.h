#pragma once
#include "Bindable.h"

class IndexBuffer : public Bindable
{

public:
	IndexBuffer(Graphics& gfx, std::vector<unsigned int> indices);
	void Bind(Graphics& gfx) noexcept override;
	UINT GetCount() const noexcept;

private:
	UINT indexCount = 0;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
};