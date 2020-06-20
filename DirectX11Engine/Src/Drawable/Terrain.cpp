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

Terrain::Terrain(Graphics& gfx, const std::string& heightMap, float scale /*= 1.0f*/) noexcept
{
	{
		terrainInfo.heightScale = scale;

		const Image& img = Image::FromFile(heightMap);

		terrainTextureDimenion = img.GetHeight();

		std::vector<Image::Color> heightMapData;
		heightMapData.reserve(img.GetPixelCount());
		heightMapData.insert(heightMapData.end(), &img.GetData()[0], &img.GetData()[img.GetPixelCount()]);
		terraintexture.reserve(img.GetPixelCount());

		for (int i = 0; i < img.GetPixelCount(); i++)
		{
			float height = ((heightMapData[i].GetR() / 255.0f) * 2.0f - 1.0f) * scale;
			terraintexture.push_back({ 1.0f, 0.5f, 0.2f, height });
		}
	}

	AddBind(Texture::Resolve(gfx, heightMap, 0, PipelineStage::DomainShader));
	AddBind(Texture::Resolve(gfx, "Resources\\Images\\testHeightMap3Normal.png", 1, PipelineStage::DomainShader));

	vertices.reserve(meshResolution);

	const unsigned int indexCount = (meshResolution - 1) * (meshResolution - 1) * 4;
	const float scaleToFit_1Km_Unit = 1000.0f / static_cast<float>(meshResolution);

	std::vector<unsigned int> indices;
	indices.reserve(indexCount);

	unsigned int skipIndex = static_cast<unsigned int>((static_cast<float>(terrainTextureDimenion) / meshResolution));
	unsigned int rowOffset = terrainTextureDimenion * skipIndex;

	int index = 0;
	for (unsigned int i = 0; i < meshResolution; i++)
	{
		for (unsigned int j = 0; j < meshResolution; j++)
		{
			float x = i * scaleToFit_1Km_Unit, z = j * scaleToFit_1Km_Unit;
			//float height = ((heightMapData[i * skipIndex + rowOffset * j].GetR() / 255.0f) * 2.0f - 1.0f) * scale;

			float u = static_cast<float>(i) / meshResolution;
			float v = static_cast<float>(j) / meshResolution;
			vertices.push_back({ { x, terraintexture[i * skipIndex + rowOffset * j].data.w, z } , {u, v},  { 0.0f, 1.0f, 0.0f } });
		}
	}

	for (int i = 0; i < meshResolution * meshResolution - meshResolution; i++)
	{
		if ((i + 1) % meshResolution == 0)
			continue;

		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + meshResolution);

		indices.push_back(i + meshResolution);
		indices.push_back(i + 1);
		indices.push_back(i + meshResolution + 1);
	}

	CalculateNormals();
	//CalculateTerrainVectors();

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


	ImageHDR imgHdr = ImageHDR::FromData(terraintexture, terrainTextureDimenion, terrainTextureDimenion);
	AddBind(std::make_shared<Texture>(gfx, imgHdr, 6, PipelineStage::PixelShader));
	//AddBind(Texture::Resolve(gfx, "Resources\\Images\\testHeightMap3Normal.png", 6));

	auto pvs = VertexShader::Resolve(gfx, "Shaders\\TerrainVertexShader.cso");
	auto pvsbc = std::static_pointer_cast<VertexShader>(pvs)->GetBytecode();

	AddBind(std::move(pvs));
	AddBind(Sampler::Resolve(gfx));
	AddBind(PixelShader::Resolve(gfx, "Shaders\\TerrainPixelShader.cso"));
	AddBind(HullShader::Resolve(gfx, "Shaders\\TerrainHullShader.cso"));
	AddBind(DomainShader::Resolve(gfx, "Shaders\\TerrainDomainShader.cso"));

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
	pDetailInfoBuffer = std::make_shared<HullConstantBuffer<DetailInfo>>(gfx, detailInfo, 0);
	AddBind(pTerrainInfoBuffer);
	AddBind(pDetailInfoBuffer);

}

DirectX::XMMATRIX Terrain::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixIdentity();
}

void Terrain::Draw(Graphics& gfx) const noexcept
{
	pTerrainInfoBuffer->Update(gfx, terrainInfo);
	pDetailInfoBuffer->Update(gfx, detailInfo);
	Drawable::Draw(gfx);
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
	}
	ImGui::End();
}

void Terrain::CalculateNormals() noexcept
{
	int pixelIndex = 0;

	const unsigned int maxNUm = terrainTextureDimenion * terrainTextureDimenion;
	for (int j = 0; j < terrainTextureDimenion - 1; j++)
	{
		for (int i = 0; i < terrainTextureDimenion; i++)
		{
			int tempIndex1 = ((j + 1) * terrainTextureDimenion) + i;      // Bottom left vertex.
			int tempIndex2 = ((j + 1) * terrainTextureDimenion) + (i + 1);  // Bottom right vertex.
			int tempIndex3 = (j * terrainTextureDimenion) + i;          // Upper left vertex.

			if (tempIndex1 < maxNUm && tempIndex2 < maxNUm && tempIndex3 < maxNUm)
			{

				// Get three vertices from the face.
				DirectX::XMFLOAT3 vertex1 = { 0, terraintexture[tempIndex1].data.w, 0 };
				DirectX::XMFLOAT3 vertex2 = { 0, terraintexture[tempIndex2].data.w, 1 };
				DirectX::XMFLOAT3 vertex3 = { -1, terraintexture[tempIndex3].data.w, 0 };


				using namespace DirectX;

				// Calculate the two vectors for this face.

				DirectX::XMVECTOR vector1 = DirectX::XMLoadFloat3(&vertex1) - DirectX::XMLoadFloat3(&vertex3);
				DirectX::XMVECTOR vector2 = DirectX::XMLoadFloat3(&vertex3) - DirectX::XMLoadFloat3(&vertex2);

				// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.

				DirectX::XMFLOAT3 normals;

				XMStoreFloat3(&normals, DirectX::XMVector3Cross(vector1, vector2));

				// Calculate the length.
				float length = (float)sqrt((normals.x * normals.x) + (normals.y * normals.y) + (normals.z * normals.z));

				// Normalize the final value for this face using the length.
				terraintexture[pixelIndex].data.x = normals.x / length;
				terraintexture[pixelIndex].data.y = normals.y / length;
				terraintexture[pixelIndex].data.z = normals.z / length;
			}

			pixelIndex++;
		}
	}

	pixelIndex = 0;
	for (int j = 0; j < terrainTextureDimenion; j++)
	{
		for (int i = 0; i < terrainTextureDimenion; i++)
		{
			// Initialize the sum.
			float sum[3];
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				int index = ((j - 1) * (terrainTextureDimenion - 1)) + (i - 1);

				sum[0] += terraintexture[index].data.x;
				sum[1] += terraintexture[index].data.y;
				sum[2] += terraintexture[index].data.z;
			}

			// Bottom right face.
			if ((i < (terrainTextureDimenion - 1)) && ((j - 1) >= 0))
			{
				int index = ((j - 1) * (terrainTextureDimenion - 1)) + i;
				sum[0] += terraintexture[index].data.x;
				sum[1] += terraintexture[index].data.y;
				sum[2] += terraintexture[index].data.z;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (terrainTextureDimenion - 1)))
			{
				int index = (j * (terrainTextureDimenion - 1)) + (i - 1);

				sum[0] += terraintexture[index].data.x;
				sum[1] += terraintexture[index].data.y;
				sum[2] += terraintexture[index].data.z;
			}

			// Upper right face.
			if ((i < (terrainTextureDimenion - 1)) && (j < (terrainTextureDimenion - 1)))
			{
				int index = (j * (terrainTextureDimenion - 1)) + i;

				sum[0] += terraintexture[index].data.x;
				sum[1] += terraintexture[index].data.y;
				sum[2] += terraintexture[index].data.z;
			}

			// Calculate the length of this normal.
			float length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			

			// Normalize the final shared normal for this vertex and store it in the height map array.
			terraintexture[pixelIndex].data.w = (sum[0] / length);
			terraintexture[pixelIndex].data.x = (sum[1] / length);
			terraintexture[pixelIndex].data.y = (sum[2] / length);

			pixelIndex++;
		}
	}



	int i, j, index1, index2, index3, index;
	DirectX::XMFLOAT3 vertex1, vertex2, vertex3;
	DirectX::XMVECTOR vector1, vector2;
	float sum[3], length;
	std::vector<DirectX::XMFLOAT3> normals((meshResolution - 1) * (meshResolution - 1));

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j < (meshResolution - 1); j++)
	{
		for (i = 0; i < (meshResolution - 1); i++)
		{
			index1 = ((j + 1) * meshResolution) + i;      // Bottom left vertex.
			index2 = ((j + 1) * meshResolution) + (i + 1);  // Bottom right vertex.
			index3 = (j * meshResolution) + i;          // Upper left vertex.

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


			index = (j * (meshResolution - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.

			XMStoreFloat3(&normals[index], DirectX::XMVector3Cross(vector1, vector2));

			// Calculate the length.
			length = (float)sqrt((normals[index].x * normals[index].x) + (normals[index].y * normals[index].y) +
				(normals[index].z * normals[index].z));

			// Normalize the final value for this face using the length.
			normals[index].x = (normals[index].x / length);
			normals[index].y = (normals[index].y / length);
			normals[index].z = (normals[index].z / length);

		}
	}

	// Now go through all the vertices and take a sum of the face normals that touch this vertex.
	for (j = 0; j < meshResolution; j++)
	{
		for (i = 0; i < meshResolution; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (meshResolution - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Bottom right face.
			if ((i < (meshResolution - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (meshResolution - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (meshResolution - 1)))
			{
				index = (j * (meshResolution - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper right face.
			if ((i < (meshResolution - 1)) && (j < (meshResolution - 1)))
			{
				index = (j * (meshResolution - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Calculate the length of this normal.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * meshResolution) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			vertices[index].normal.x = (sum[0] / length);
			vertices[index].normal.y = (sum[1] / length);
			vertices[index].normal.z = (sum[2] / length);
		}
	}

}

void Terrain::CalculateTerrainVectors() noexcept
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

	// Calculate the length of the tangent.
	//length = DirectX::XMVector3Length(tangent).m128_f32[0];//(float)sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the tangent and then store it.

	tangent = DirectX::XMVector3Normalize(tangent);

	// Calculate the length of the binormal.
	//length = (float)sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the binormal and then store it.

	binormal = DirectX::XMVector3Normalize(binormal);
}
