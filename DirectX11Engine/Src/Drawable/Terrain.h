#pragma once
#include "Drawable.h"
#include "../Bindable/ConstantBuffers.h"
#include "../Image.h"


class Terrain : public Drawable
{
public:
	Terrain(Graphics& gfx, const std::string& heightMap, float scale = 1.0f) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Draw(Graphics& gfx) const noexcept override;
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT3 binormal;
	};

	void CalculateNormals() noexcept;
	std::vector<Image::Color> heightMapData;
	std::vector<Vertex> vertices;

	static constexpr unsigned int meshResolution = 512;

};

