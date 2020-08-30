#pragma once
#include "Drawable.h"
#include "../Bindable/BindableCommon.h"
#include "../Image.h"
#include "Model.h"

class Terrain : public Drawable
{
public:

	struct TerrainInitInfo
	{
		unsigned int baseMeshResolution = 64; // Values above 512 might be redundant as tessellation will provide detail up close
		float terrainUnitScale = 500.0f; // The world unit it extends in meters
		float heightScale = 100.0f; // Height scale in meters
	};

	struct TerrainInfo
	{
		DirectX::XMFLOAT4 colourBlendInfo[3] =
		{
			{1.0f, 1.0f, 1.0f, 0.0f},
			{1.0f, 1.0f, 1.0f, 0.2f},
			{1.0f, 1.0f, 1.0f, 0.1f}
		};
		float texScale[3] = { 0.02f, 0.1f, 0.1f };
		float padding = 0.0f;
		float startHeight[3] = { 0.0f, 0.3f, 0.6f };
		float heightScale = 0.0f;
	};

	struct DetailInfo
	{
		float maxTessellationAmount = 32.0f; // Can go up to 64
		float minDistance = 150.0f;
		float maxDistance = 500.0f;
		int smoothing = 4; // Can go up to 8
	};

	Terrain(Graphics& gfx, const std::string& heightMap, const TerrainInitInfo& initInfo) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Draw(Graphics& gfx) const noexcept override;
	void ShowWindow(const char* windowName) noexcept;
	std::shared_ptr<Texture> GetTerrainTexture() noexcept;
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 tex;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT3 binormal;
	};

	void CalculateNormals(std::vector<ImageHDR::ColorFloat>& terraintexture, std::vector<Vertex>& vertices) noexcept;
	void CalculateTerrainVectors(std::vector<ImageHDR::ColorFloat>& terraintexture, std::vector<Vertex>& vertices) noexcept;
	void CalculateTangentBinormal(Vertex vertex1, Vertex vertex2, Vertex vertex3, DirectX::XMVECTOR& tangent, DirectX::XMVECTOR& binormal) noexcept;

	unsigned int terrainTextureDimenion = 0;
	mutable bool smoothingRequiresCalc = false;

	TerrainInitInfo terrainInitInfo;
	TerrainInfo terrainInfo;
	DetailInfo detailInfo;
	ImageHDR imgHdr;
	std::shared_ptr<PixelConstantBuffer<TerrainInfo>> pTerrainInfoBuffer = nullptr;
	std::shared_ptr<VertexConstantBuffer<DetailInfo>> pDetailInfoBufferVertex = nullptr;
	std::shared_ptr<HullConstantBuffer<DetailInfo>> pDetailInfoBufferHull = nullptr;
	std::shared_ptr<DomainConstantBuffer<DetailInfo>> pDetailInfoBufferDomain = nullptr;
	std::shared_ptr<Texture> pTerrainTexture = nullptr;
	std::unique_ptr<InstanceModel> instancedFoliage;
};

