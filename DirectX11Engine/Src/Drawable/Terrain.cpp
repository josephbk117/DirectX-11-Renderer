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

Terrain::Terrain(Graphics& gfx, const std::string& heightMap, float scale /*= 1.0f*/) noexcept
{
	info.heightScale = scale;

	const Image& img = Image::FromFile(heightMap);

	heightMapData.reserve(img.GetPixelCount());
	heightMapData.insert(heightMapData.end(), &img.GetData()[0], &img.GetData()[img.GetPixelCount()]);

	namespace dx = DirectX;

	vertices.reserve(meshResolution);

	const unsigned int indexCount = (meshResolution - 1) * (meshResolution - 1) * 4;
	const float scaleToFit_1Km_Unit = 1000.0f / 512.0f;

	std::vector<unsigned int> indices;
	indices.reserve(indexCount);

	int index = 0;
	for (unsigned int i = 0; i < meshResolution; i++)
	{
		for (unsigned int j = 0; j < meshResolution; j++)
		{
			float x = i * scaleToFit_1Km_Unit, z = j * scaleToFit_1Km_Unit;
			float height = ((heightMapData[i + meshResolution * j].GetR() / 255.0f) * 2.0f - 1.0f) * scale;
			vertices.push_back({ { x, height, z } , { 0.0f, 1.0f, 0.0f } });
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

	AddBind(std::make_shared<VertexBuffer>(gfx, vertices));
	AddBind(std::make_shared<IndexBuffer>(gfx, indices));
	AddBind(Rasterizer::Resolve(gfx, Rasterizer::RasterizerMode{ false, false }));

	std::vector<std::string> paths(3);
	paths[0] = "Resources\\Images\\RocksArid0038_1_600.jpg";
	paths[1] = "Resources\\Images\\Seamless stone cliff face mountain.jpg";
	paths[2] = "Resources\\Images\\Cliffs0073_1_600.jpg";

	AddBind(TextureArray::Resolve(gfx, paths, 0));

	auto pvs = VertexShader::Resolve(gfx, "Shaders\\TerrainVertexShader.cso");
	auto pvsbc = std::static_pointer_cast<VertexShader>(pvs)->GetBytecode();

	AddBind(std::move(pvs));
	AddBind(Sampler::Resolve(gfx));
	AddBind(PixelShader::Resolve(gfx, "Shaders\\TerrainPixelShader.cso"));

	const std::vector< D3D11_INPUT_ELEMENT_DESC >ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12u,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Tangent",0,DXGI_FORMAT_R32G32B32_FLOAT,0,24u,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "BiTangent",0,DXGI_FORMAT_R32G32B32_FLOAT,0,36u,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	AddBind(std::make_shared<InputLayout>(gfx, ied, pvsbc));
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::make_shared<TransformCbuffer>(gfx, *this, 0));


	pTerrainInfoBuffer = std::make_shared<PixelConstantBuffer<TerrainInfo>>(gfx, info, 0);
	AddBind(pTerrainInfoBuffer);

}

DirectX::XMMATRIX Terrain::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixIdentity();
}

void Terrain::Draw(Graphics& gfx) const noexcept
{
	pTerrainInfoBuffer->Update(gfx, info);
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
			ImGui::SliderFloat(sliderName.c_str(), &info.startHeight[i], 0.0f, 1.0f);

			const std::string texScale = "Texture Scale #"s + std::to_string(i);
			ImGui::SliderFloat(texScale.c_str(), &info.texScale[i], -1.0f, 1.0f);

			const std::string colourAndBlend = "Colour And Blend #"s + std::to_string(i);
			DirectX::XMVECTOR col = DirectX::XMLoadFloat4(&info.colourBlendInfo[i]);
			ImGui::ColorEdit4(colourAndBlend.c_str(), &col.m128_f32[0]);
			DirectX::XMStoreFloat4(&info.colourBlendInfo[i], col);
		}
	}
	ImGui::End();
}

void Terrain::CalculateNormals() noexcept
{

	int i, j, index1, index2, index3, index;
	DirectX::XMFLOAT3 vertex1, vertex2, vertex3;
	DirectX::XMVECTOR vector1, vector2;
	float sum[3], length;
	std::vector<DirectX::XMFLOAT3> normals(511 * 511);

	const unsigned int m_terrainWidth = 512, m_terrainHeight = 512;

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j < (512 - 1); j++)
	{
		for (i = 0; i < (512 - 1); i++)
		{
			index1 = ((j + 1) * m_terrainWidth) + i;      // Bottom left vertex.
			index2 = ((j + 1) * m_terrainWidth) + (i + 1);  // Bottom right vertex.
			index3 = (j * m_terrainWidth) + i;          // Upper left vertex.

			// Get three vertices from the face.
			vertex1 = vertices[index1].pos;
			vertex2 = vertices[index2].pos;
			vertex3 = vertices[index3].pos;

			using namespace DirectX;

			// Calculate the two vectors for this face.


			vector1 = DirectX::XMLoadFloat3(&vertex1) - DirectX::XMLoadFloat3(&vertex3);
			vector2 = DirectX::XMLoadFloat3(&vertex3) - DirectX::XMLoadFloat3(&vertex2);


			index = (j * (m_terrainWidth - 1)) + i;

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
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Bottom right face.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper right face.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Calculate the length of this normal.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainWidth) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			vertices[index].normal.x = (sum[0] / length);
			vertices[index].normal.y = (sum[1] / length);
			vertices[index].normal.z = (sum[2] / length);
		}
	}

}
