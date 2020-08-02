#include "Terrain.h"
#include "../Image.h"
#include "../Bindable/Bindable.h"
#include "../Bindable/Topology.h"
#include "../Bindable/TransformCBuffer.h"
#include "../Bindable/TransformCBufferEx.h"
#include "../Bindable/VertexBuffer.h"
#include "../Bindable/VertexShader.h"
#include "../Bindable/Sampler.h"
#include "../Bindable/PixelShader.h"
#include "../Bindable/InputLayout.h"
#include "../Bindable/Texture.h"
#include "../Bindable/IndexBuffer.h"
#include "../Bindable/BlendOperation.h"
#include "../Bindable/Rasterizer.h"
#include "../Imgui/imgui.h"
#include "../Bindable/TextureArray.h"
#include "../Bindable/HullShader.h"
#include "../Bindable/DomainShader.h"
#include "../Bindable/ComputeShader.h"

Terrain::Terrain(Graphics& gfx, const std::string& heightMap, const TerrainInitInfo& initInfo) noexcept : terrainInitInfo(initInfo)
{
	std::vector<ImageHDR::ColorFloat> terraintexture;

	{
		terrainInfo.heightScale = initInfo.heightScale;

		const Image& img = Image::FromFile(heightMap);

		terrainTextureDimenion = img.GetHeight();

		std::vector<Image::Color> heightMapData;
		heightMapData.reserve(img.GetPixelCount());
		heightMapData.insert(heightMapData.end(), &img.GetData()[0], &img.GetData()[img.GetPixelCount()]);
		terraintexture.reserve(img.GetPixelCount());

		for (int i = 0; i < img.GetPixelCount(); i++)
		{
			const float height = ((heightMapData[i].GetR() / 255.0f) * 2.0f - 1.0f) * initInfo.heightScale;
			terraintexture.push_back({ height });
		}
		
	}

	std::vector<Vertex> vertices;
	vertices.reserve(initInfo.baseMeshResolution);

	const unsigned int indexCount = (initInfo.baseMeshResolution - 1) * (initInfo.baseMeshResolution - 1) * 4;
	const float scaleToFitTerrainUnit = initInfo.terrainUnitScale / static_cast<float>(initInfo.baseMeshResolution);

	std::vector<unsigned int> indices;
	indices.reserve(indexCount);

	unsigned int skipIndex = static_cast<unsigned int>((static_cast<float>(terrainTextureDimenion) / initInfo.baseMeshResolution));
	unsigned int rowOffset = terrainTextureDimenion * skipIndex;

	for (unsigned int i = 0; i < initInfo.baseMeshResolution; i++)
	{
		for (unsigned int j = 0; j < initInfo.baseMeshResolution; j++)
		{
			float x = i * scaleToFitTerrainUnit, z = j * scaleToFitTerrainUnit;
			float u = static_cast<float>(i) / initInfo.baseMeshResolution;
			float v = static_cast<float>(j) / initInfo.baseMeshResolution;
			vertices.push_back({ { x, 0, z } , {u, v},  { 0.0f, 1.0f, 0.0f } });
		}
	}

	for (int i = 0; i < initInfo.baseMeshResolution * initInfo.baseMeshResolution - initInfo.baseMeshResolution; i++)
	{
		if ((i + 1) % initInfo.baseMeshResolution == 0)
			continue;

		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + initInfo.baseMeshResolution);

		indices.push_back(i + initInfo.baseMeshResolution);
		indices.push_back(i + 1);
		indices.push_back(i + initInfo.baseMeshResolution + 1);
	}

	CalculateNormals(terraintexture, vertices);

	AddBind(std::make_shared<VertexBuffer>(gfx, vertices));
	AddBind(std::make_shared<IndexBuffer>(gfx, indices));
	AddBind(Rasterizer::Resolve(gfx, Rasterizer::RasterizerMode{ false, false }));

	std::vector<std::string> paths(3);
	paths[1] = "Resources\\Images\\Triplanar set\\ompjR_4K_Albedo.jpg";
	paths[0] = "Resources\\Images\\Triplanar set\\tefndhwq_4K_Albedo.jpg";
	paths[2] = "Resources\\Images\\Triplanar set\\ueumdayn_2K_Albedo.jpg";

	AddBind(TextureArray::Resolve(gfx, paths, 0));

	paths[1] = "Resources\\Images\\Triplanar set\\ompjR_4K_Normal.jpg";
	paths[0] = "Resources\\Images\\Triplanar set\\tefndhwq_4K_Normal.jpg";
	paths[2] = "Resources\\Images\\Triplanar set\\ueumdayn_2K_Normal.jpg";

	AddBind(TextureArray::Resolve(gfx, paths, 3));

	imgHdr = ImageHDR::FromData(terraintexture, terrainTextureDimenion, terrainTextureDimenion);

	std::vector<PipelineStageSlotInfo> pipelineStageInfos;
	pipelineStageInfos.push_back({ PipelineStage::PixelShader , 6 });
	pipelineStageInfos.push_back({ PipelineStage::DomainShader , 0 });

	pTerrainTexture = std::make_shared<Texture>(gfx, imgHdr, pipelineStageInfos, false);
	AddBind(pTerrainTexture);

	auto pvs = VertexShader::Resolve(gfx, "Shaders\\TerrainVertexShader.cso");
	auto pvsbc = std::static_pointer_cast<VertexShader>(pvs)->GetBytecode();

	AddBind(std::move(pvs));
	AddBind(Sampler::Resolve(gfx));
	AddBind(PixelShader::Resolve(gfx, "Shaders\\TerrainPixelShader.cso"));
	AddBind(HullShader::Resolve(gfx, "Shaders\\TerrainHullShader.cso"));
	AddBind(DomainShader::Resolve(gfx, "Shaders\\TerrainDomainShader.cso"));
	AddBind(ComputeShader::Resolve(gfx, "Shaders\\TerrainComputeShader.cso"));

	const std::vector< D3D11_INPUT_ELEMENT_DESC >ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT,0,12u,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,20u,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Tangent",0,DXGI_FORMAT_R32G32B32_FLOAT,0,32u,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "BiTangent",0,DXGI_FORMAT_R32G32B32_FLOAT,0,44u,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	AddBind(std::make_shared<InputLayout>(gfx, ied, pvsbc));
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST));
	AddBind(std::make_shared<TransformCbuffer>(gfx, *this, 0));


	pTerrainInfoBuffer = std::make_shared<PixelConstantBuffer<TerrainInfo>>(gfx, terrainInfo, 0);
	pDetailInfoBufferVertex = std::make_shared<VertexConstantBuffer<DetailInfo>>(gfx, detailInfo, 1);
	pDetailInfoBufferHull = std::make_shared<HullConstantBuffer<DetailInfo>>(gfx, detailInfo, 0);
	pDetailInfoBufferDomain = std::make_shared<DomainConstantBuffer<DetailInfo>>(gfx, detailInfo, 0);

	AddBind(pTerrainInfoBuffer);
	AddBind(pDetailInfoBufferVertex);
	AddBind(pDetailInfoBufferHull);
	AddBind(pDetailInfoBufferDomain);

}

DirectX::XMMATRIX Terrain::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixIdentity();
}

void Terrain::Draw(Graphics& gfx) const noexcept
{
	pTerrainInfoBuffer->Update(gfx, terrainInfo);
	pDetailInfoBufferVertex->Update(gfx, detailInfo);
	pDetailInfoBufferHull->Update(gfx, detailInfo);
	pDetailInfoBufferDomain->Update(gfx, detailInfo);
	Drawable::Draw(gfx);
	pTerrainTexture->UnBind(gfx);
	pTerrainTexture->CustomBind(gfx, { PipelineStage::ComputeShader, 0 });
	gfx.Dispatch(64, 64, 1);
	pTerrainTexture->CustomUnBind(gfx, { PipelineStage::ComputeShader, 0 });
}

void Terrain::ShowWindow(const char* windowName) noexcept
{
	windowName = windowName ? windowName : "Model";
	using namespace std::string_literals;
	if (ImGui::Begin(windowName))
	{
		ImGui::Text("Terrain Info");

		for (int i = 0; i < 3; i++)
		{
			const std::string sliderName = "Start Height #"s + std::to_string(i);
			ImGui::SliderFloat(sliderName.c_str(), &terrainInfo.startHeight[i], 0.0f, 1.0f);

			const std::string texScale = "Texture Scale #"s + std::to_string(i);
			ImGui::SliderFloat(texScale.c_str(), &terrainInfo.texScale[i], -1.0f, 1.0f);

			const std::string colourAndBlend = "Colour And Blend #"s + std::to_string(i);
			DirectX::XMVECTOR col = DirectX::XMLoadFloat4(&terrainInfo.colourBlendInfo[i]);
			ImGui::ColorEdit4(colourAndBlend.c_str(), &col.m128_f32[0]);
			DirectX::XMStoreFloat4(&terrainInfo.colourBlendInfo[i], col);
		}

		ImGui::Separator();
		ImGui::SliderFloat("Tessellation Factor", &detailInfo.maxTessellationAmount, 1.0f, 64.0f);
		ImGui::SliderFloat("Max Tessellation Distance", &detailInfo.maxDistance, 50.0f, 2500.0f);
	}
	ImGui::End();
}

ImageHDR* Terrain::GetTerrainTexture() noexcept
{
	return &imgHdr;
}

void Terrain::CalculateNormals(std::vector<ImageHDR::ColorFloat>& terraintexture, std::vector<Vertex>& vertices) noexcept
{

	{
		const float scaleToFitTerrainUnit = terrainInitInfo.terrainUnitScale / static_cast<float>(terrainTextureDimenion);

		std::vector<DirectX::XMFLOAT3> vertices;

		for (unsigned int i = 0; i < terrainTextureDimenion; i++)
		{
			for (unsigned int j = 0; j < terrainTextureDimenion; j++)
			{
				float x = i * scaleToFitTerrainUnit, z = j * scaleToFitTerrainUnit;
				float u = static_cast<float>(i) / terrainTextureDimenion;
				float v = static_cast<float>(j) / terrainTextureDimenion;
				vertices.push_back({ x, terraintexture[i + terrainTextureDimenion * j].data.w, z });
			}
		}

		//-----------------

		int i, j, index1, index2, index3, index;
		DirectX::XMFLOAT3 vertex1, vertex2, vertex3;
		DirectX::XMVECTOR vector1, vector2;
		float sum[3], length;
		std::vector<DirectX::XMFLOAT3> normals((terrainTextureDimenion - 1u) * (terrainTextureDimenion - 1u));

		int pixelIndex = 0;

		// Go through all the faces in the mesh and calculate their normals.
		for (j = 0; j < (terrainTextureDimenion - 1); j++)
		{
			for (i = 0; i < (terrainTextureDimenion - 1); i++)
			{
				index1 = ((j + 1) * terrainTextureDimenion) + i;      // Bottom left vertex.
				index2 = ((j + 1) * terrainTextureDimenion) + (i + 1);  // Bottom right vertex.
				index3 = (j * terrainTextureDimenion) + i;          // Upper left vertex.

				// Get three vertices from the face.
				vertex1 = vertices[index1];
				vertex2 = vertices[index2];
				vertex3 = vertices[index3];


				using namespace DirectX;

				// Calculate the two vectors for this face.

				vector1 = DirectX::XMLoadFloat3(&vertex1) - DirectX::XMLoadFloat3(&vertex3);
				vector2 = DirectX::XMLoadFloat3(&vertex3) - DirectX::XMLoadFloat3(&vertex2);

				index = (j * (terrainTextureDimenion - 1)) + i;

				// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.

				XMStoreFloat3(&normals[index], DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vector1, vector2)));
			}
		}

		std::vector<DirectX::XMFLOAT3> vertNormals(terrainTextureDimenion * terrainTextureDimenion);

		// Now go through all the vertices and take a sum of the face normals that touch this vertex.
		for (j = 0; j < terrainTextureDimenion; j++)
		{
			for (i = 0; i < terrainTextureDimenion; i++)
			{
				// Initialize the sum.
				sum[0] = 0.0f;
				sum[1] = 0.0f;
				sum[2] = 0.0f;

				// Bottom left face.
				if (((i - 1) >= 0) && ((j - 1) >= 0))
				{
					index = ((j - 1) * (terrainTextureDimenion - 1)) + (i - 1);

					sum[0] += normals[index].x;
					sum[1] += normals[index].y;
					sum[2] += normals[index].z;
				}

				// Bottom right face.
				if ((i < (terrainTextureDimenion - 1)) && ((j - 1) >= 0))
				{
					index = ((j - 1) * (terrainTextureDimenion - 1)) + i;

					sum[0] += normals[index].x;
					sum[1] += normals[index].y;
					sum[2] += normals[index].z;
				}

				// Upper left face.
				if (((i - 1) >= 0) && (j < (terrainTextureDimenion - 1)))
				{
					index = (j * (terrainTextureDimenion - 1)) + (i - 1);

					sum[0] += normals[index].x;
					sum[1] += normals[index].y;
					sum[2] += normals[index].z;
				}

				// Upper right face.
				if ((i < (terrainTextureDimenion - 1)) && (j < (terrainTextureDimenion - 1)))
				{
					index = (j * (terrainTextureDimenion - 1)) + i;

					sum[0] += normals[index].x;
					sum[1] += normals[index].y;
					sum[2] += normals[index].z;
				}

				// Calculate the length of this normal.
				length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

				// Get an index to the vertex location in the height map array.
				index = (j * terrainTextureDimenion) + i;

				// Normalize the final shared normal for this vertex and store it in the height map array.
				vertNormals[index].x = (sum[0] / length);
				vertNormals[index].y = (sum[1] / length);
				vertNormals[index].z = (sum[2] / length);
			}
		}


		for (i = 0; i < terrainTextureDimenion; i++)
		{
			for (j = 0; j < terrainTextureDimenion; j++)
			{
				int index1 = (j * terrainTextureDimenion) + i;
				int index2 = (i * terrainTextureDimenion) + j;

				terraintexture[index1].SetR(vertNormals[index2].x);
				terraintexture[index1].SetG(vertNormals[index2].y);
				terraintexture[index1].SetB(vertNormals[index2].z);
			}
		}

	}

	int i, j, index1, index2, index3, index;
	DirectX::XMFLOAT3 vertex1, vertex2, vertex3;
	DirectX::XMVECTOR vector1, vector2;
	float sum[3], length;
	std::vector<DirectX::XMFLOAT3> normals((terrainInitInfo.baseMeshResolution - 1u) * (terrainInitInfo.baseMeshResolution - 1u));

	int pixelIndex = 0;

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j < (terrainInitInfo.baseMeshResolution - 1); j++)
	{
		for (i = 0; i < (terrainInitInfo.baseMeshResolution - 1); i++)
		{
			index1 = ((j + 1) * terrainInitInfo.baseMeshResolution) + i;      // Bottom left vertex.
			index2 = ((j + 1) * terrainInitInfo.baseMeshResolution) + (i + 1);  // Bottom right vertex.
			index3 = (j * terrainInitInfo.baseMeshResolution) + i;          // Upper left vertex.

			// Get three vertices from the face.
			vertex1 = vertices[index1].pos;
			vertex2 = vertices[index2].pos;
			vertex3 = vertices[index3].pos;

			DirectX::XMVECTOR tangent, biTan;
			CalculateTangentBinormal(vertices[index1], vertices[index2], vertices[index3], tangent, biTan);

			DirectX::XMStoreFloat3(&vertices[index1].tangent, tangent);
			DirectX::XMStoreFloat3(&vertices[index1].binormal, biTan);

			using namespace DirectX;

			// Calculate the two vectors for this face.

			vector1 = DirectX::XMLoadFloat3(&vertex1) - DirectX::XMLoadFloat3(&vertex3);
			vector2 = DirectX::XMLoadFloat3(&vertex3) - DirectX::XMLoadFloat3(&vertex2);

			index = (j * (terrainInitInfo.baseMeshResolution - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.

			XMStoreFloat3(&normals[index], DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vector1, vector2)));
		}
	}

	// Now go through all the vertices and take a sum of the face normals that touch this vertex.
	for (j = 0; j < terrainInitInfo.baseMeshResolution; j++)
	{
		for (i = 0; i < terrainInitInfo.baseMeshResolution; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (terrainInitInfo.baseMeshResolution - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Bottom right face.
			if ((i < (terrainInitInfo.baseMeshResolution - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (terrainInitInfo.baseMeshResolution - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (terrainInitInfo.baseMeshResolution - 1)))
			{
				index = (j * (terrainInitInfo.baseMeshResolution - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper right face.
			if ((i < (terrainInitInfo.baseMeshResolution - 1)) && (j < (terrainInitInfo.baseMeshResolution - 1)))
			{
				index = (j * (terrainInitInfo.baseMeshResolution - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Calculate the length of this normal.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * terrainInitInfo.baseMeshResolution) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			vertices[index].normal.x = (sum[0] / length);
			vertices[index].normal.y = (sum[1] / length);
			vertices[index].normal.z = (sum[2] / length);
		}
	}
}

void Terrain::CalculateTerrainVectors(std::vector<ImageHDR::ColorFloat>& terraintexture, std::vector<Vertex>& vertices) noexcept
{
	unsigned int faceCount, i, index;
	Vertex vertex1, vertex2, vertex3;
	DirectX::XMVECTOR tangent, binormal;


	// Calculate the number of faces in the terrain model.
	faceCount = vertices.size() / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (i = 0; i < faceCount; i++)
	{
		// Get the three vertices for this face from the terrain model.

		vertex1 = vertices[index];
		index++;

		vertex2 = vertices[index];
		index++;

		vertex3 = vertices[index];
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Store the tangent and binormal for this face back in the model structure.
		DirectX::XMStoreFloat3(&vertices[index - 1u].tangent, tangent);
		DirectX::XMStoreFloat3(&vertices[index - 1u].binormal, binormal);

		DirectX::XMStoreFloat3(&vertices[index - 2u].tangent, tangent);
		DirectX::XMStoreFloat3(&vertices[index - 2u].binormal, binormal);

		DirectX::XMStoreFloat3(&vertices[index - 3u].tangent, tangent);
		DirectX::XMStoreFloat3(&vertices[index - 3u].binormal, binormal);

	}
}

void Terrain::CalculateTangentBinormal(Vertex vertex1, Vertex vertex2, Vertex vertex3, DirectX::XMVECTOR& tangent, DirectX::XMVECTOR& binormal) noexcept
{
	DirectX::XMVECTOR vector1, vector2;
	DirectX::XMVECTOR tuVector, tvVector;
	float den;
	float length;

	using namespace DirectX;

	// Calculate the two vectors for this face.

	vector1 = DirectX::XMLoadFloat3(&vertex1.pos) - DirectX::XMLoadFloat3(&vertex3.pos);
	vector2 = DirectX::XMLoadFloat3(&vertex3.pos) - DirectX::XMLoadFloat3(&vertex2.pos);

	// Calculate the tu and tv texture space vectors.

	tuVector = DirectX::XMLoadFloat2(&vertex1.tex) - DirectX::XMLoadFloat2(&vertex2.tex);
	tvVector = DirectX::XMLoadFloat2(&vertex1.tex) - DirectX::XMLoadFloat2(&vertex3.tex);

	// Calculate the denominator of the tangent/binormal equation.
	//den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);
	den = 1.0f / (tuVector.m128_f32[0] * tvVector.m128_f32[1] - tuVector.m128_f32[1] * tvVector.m128_f32[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.m128_f32[0] = (tvVector.m128_f32[1] * vector1.m128_f32[0] - tvVector.m128_f32[0] * vector2.m128_f32[0]) * den;
	tangent.m128_f32[1] = (tvVector.m128_f32[1] * vector1.m128_f32[1] - tvVector.m128_f32[0] * vector2.m128_f32[1]) * den;
	tangent.m128_f32[2] = (tvVector.m128_f32[1] * vector1.m128_f32[2] - tvVector.m128_f32[0] * vector2.m128_f32[2]) * den;

	binormal.m128_f32[0] = (tuVector.m128_f32[0] * vector2.m128_f32[0] - tuVector.m128_f32[1] * vector1.m128_f32[0]) * den;
	binormal.m128_f32[1] = (tuVector.m128_f32[0] * vector2.m128_f32[1] - tuVector.m128_f32[1] * vector1.m128_f32[1]) * den;
	binormal.m128_f32[2] = (tuVector.m128_f32[0] * vector2.m128_f32[2] - tuVector.m128_f32[1] * vector1.m128_f32[2]) * den;

	// Normalize the tangent, bitangent and then store it.

	tangent = DirectX::XMVector3Normalize(tangent);
	binormal = DirectX::XMVector3Normalize(binormal);
}
