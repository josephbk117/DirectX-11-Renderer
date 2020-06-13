#pragma once
#include "Drawable.h"
#include "../Bindable/ConstantBuffers.h"
#include "../Image.h"


class Terrain : public Drawable
{
public:

	struct TerrainInfo
	{
		DirectX::XMFLOAT4 colourBlendInfo[3] =
		{
			{1.0f, 0.0f, 0.0f, 0.2f},
			{0.0f, 1.0f, 0.0f, 0.2f},
			{0.0f, 0.0f, 1.0f, 0.5f}
		};
		float texScale[3] = { 0.02f, 0.2f, 0.2f };
		float padding;
		float startHeight[3] = { 0.0f, 0.4f, 0.7f };
		float heightScale = 0.0f;
	};

	struct DetailInfo
	{
		float maxTessellationAmount = 1.0f;
		float padding[3];
	};

	Terrain(Graphics& gfx, const std::string& heightMap, float scale = 1.0f) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Draw(Graphics& gfx) const noexcept override;
	void ShowWindow(const char* windowName) noexcept;
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
	TerrainInfo terrainInfo;
	DetailInfo detailInfo;
	std::shared_ptr<PixelConstantBuffer<TerrainInfo>> pTerrainInfoBuffer = nullptr;
	std::shared_ptr<HullConstantBuffer<DetailInfo>> pDetailInfoBuffer = nullptr;

	static constexpr unsigned int meshResolution = 256;

};

