#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(Graphics& gfx, std::vector<unsigned int> indices) : indexCount((UINT)indices.size())
{
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = UINT(indexCount * sizeof(unsigned int));
	bd.StructureByteStride = sizeof(unsigned int);

	D3D11_SUBRESOURCE_DATA srd;
	srd.pSysMem = indices.data();

	GetDevice(gfx)->CreateBuffer(&bd, &srd, &pIndexBuffer);
}

void IndexBuffer::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

UINT IndexBuffer::GetCount() const noexcept
{
	return indexCount;
}
