#include "PhysicalSkybox.h"
#include "../Bindable/BindableCommon.h"
#include "../Bindable/NullHullShader.h"
#include "../Bindable/NullDomainShader.h"
#include "../Bindable/TransformCBuffer.h"
#include "../Bindable/TransformCBufferEx.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../Imgui/imgui.h"


PhysicalSkybox::PhysicalSkybox(Graphics& gfx, const std::string& fileName)
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName, aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded
	);

	namespace dx = DirectX;
	struct Vertex
	{
		dx::XMFLOAT3 pos;
	};

	std::vector<Vertex> vertices;

	aiMesh& mesh = *imp.GetScene()->mMeshes[0];

	vertices.reserve(mesh.mNumVertices);

	const float scale = 2000.0f;

	for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
	{
		vertices.push_back
		(
			{ { mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale }, }
		);
	}

	std::vector<unsigned int> indices;
	indices.reserve(static_cast<size_t>(mesh.mNumFaces) * 3);

	for (unsigned int i = 0; i < mesh.mNumFaces; ++i)
	{
		const auto& face = mesh.mFaces[i];

		assert(face.mNumIndices == 3);

		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	AddBind(std::make_shared<VertexBuffer>(gfx, vertices));
	AddBind(std::make_shared<IndexBuffer>(gfx, indices));
	AddBind(Rasterizer::Resolve(gfx, Rasterizer::RasterizerMode{ false, false }));

	auto pvs = VertexShader::Resolve(gfx, "Shaders\\SkyboxVertexShader.cso");
	auto pvsbc = std::static_pointer_cast<VertexShader>(pvs)->GetBytecode();

	AddBind(std::move(pvs));
	SamplerSettings samplerSettings
	{
		D3D11_FILTER::D3D11_FILTER_ANISOTROPIC,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP
	};
	std::vector<PipelineStageSlotInfo> samplerPipelineInfo;
	samplerPipelineInfo.push_back({ PipelineStage::PixelShader, 0 });
	AddBind(Sampler::Resolve(gfx, samplerSettings, samplerPipelineInfo));
	AddBind(PixelShader::Resolve(gfx, "Shaders\\SkyboxPixelShader.cso"));

	AddBind(NullHullShader::Resolve(gfx));
	AddBind(NullDomainShader::Resolve(gfx));

	const std::vector< D3D11_INPUT_ELEMENT_DESC >ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	AddBind(std::make_shared<InputLayout>(gfx, ied, pvsbc));
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::make_shared<TransformCbuffer>(gfx, *this, 0));

	pSkyInfoBuffer = std::make_shared<PixelConstantBuffer<SkyInfo>>(gfx, skyInfo, 0);
	AddBind(pSkyInfoBuffer);
}

DirectX::XMMATRIX PhysicalSkybox::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixIdentity();
}

void PhysicalSkybox::Draw(Graphics& gfx) const noexcept
{
	Drawable::Draw(gfx);
}

void PhysicalSkybox::Update(Graphics& gfx) noexcept
{
	pSkyInfoBuffer->Update(gfx, skyInfo);
}

void PhysicalSkybox::ShowWindow(const char* windowName) noexcept
{
	windowName = windowName ? windowName : "Model";
	if (ImGui::Begin(windowName))
	{
		ImGui::Text("Sky Info");

		ImGui::SliderFloat("Rayleigh Density", &skyInfo.rayleighDensity, 0.0f, 50000.0f);
		ImGui::SliderFloat("Mie Density", &skyInfo.mieDensity, 0.0f, 50000.0f);
		ImGui::SliderFloat("Height", &skyInfo.height, 1.0f, 100000.0f);
		ImGui::SliderFloat("Sun Intensity", &skyInfo.sunPower, 0.0f, 100.0f);


		static float vals[2] = { 22.0f / 7.0f, 0.0f };
		ImGui::SliderFloat2("Sun Direction", vals, 0.0f, 22.0f / 7.0f * 2.0f);
		DirectX::XMVECTOR sunDirVec = DirectX::XMVector3Transform({ 0.0f, 1.0f, 0.0f }, DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMVECTOR{ vals[0] - 22.0f / 7.0f, vals[1], 0.0f }));
		DirectX::XMStoreFloat3(&skyInfo.sunDir, sunDirVec);

		DirectX::XMVECTOR col = DirectX::XMLoadFloat3(&skyInfo.groundColour);
		ImGui::ColorEdit4("Ground Colour", &col.m128_f32[0]);
		DirectX::XMStoreFloat3(&skyInfo.groundColour, col);

	}
	ImGui::End();

}
