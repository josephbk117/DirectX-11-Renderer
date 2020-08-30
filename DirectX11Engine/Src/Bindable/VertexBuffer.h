#pragma once
#include "Bindable.h"

class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer(Graphics& gfx, const std::vector<V>& vertices, BufferUsage usage = BufferUsage::LongLived) : stride(sizeof(V))
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = (D3D11_USAGE)usage;
		bd.CPUAccessFlags = (usage == BufferUsage::LongLived || usage == BufferUsage::Immutable) ? 0u : D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(sizeof(V) * vertices.size());
		bd.StructureByteStride = sizeof(V);
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices.data();
		GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer);

	}

	template<typename V>
	void UpdateBuffer(Graphics& gfx, const std::vector<V>& vertices)
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		GetContext(gfx)->Map(pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, vertices.data(), stride * vertices.size());
		GetContext(gfx)->Unmap(pVertexBuffer.Get(), 0);
	}
	void Bind(Graphics& gfx) noexcept override;
protected:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};