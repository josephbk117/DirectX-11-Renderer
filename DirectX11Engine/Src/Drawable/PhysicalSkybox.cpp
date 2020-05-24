#include "PhysicalSkybox.h"
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
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

	const float scale = 1000.0f;

	for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
	{
		vertices.push_back
		(
			{{ mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale },}
		);
	}

	std::vector<unsigned short> indices;
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
	AddBind(Rasterizer::Resolve(gfx, Rasterizer::RasterizerMode{ true, false }));

	auto pvs = VertexShader::Resolve(gfx, "Shaders\\SkyboxVertexShader.cso");
	auto pvsbc = std::static_pointer_cast<VertexShader>(pvs)->GetBytecode();

	AddBind(std::move(pvs));
	AddBind(Sampler::Resolve(gfx));
	AddBind(PixelShader::Resolve(gfx, "Shaders\\SkyboxPixelShader.cso"));

	const std::vector< D3D11_INPUT_ELEMENT_DESC >ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	AddBind(std::make_shared<InputLayout>(gfx, ied, pvsbc));
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::make_shared<TransformCBufferEx>(gfx, *this, 0));
}

DirectX::XMMATRIX PhysicalSkybox::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixIdentity();
}

void PhysicalSkybox::Draw(Graphics& gfx) const noexcept
{
	Drawable::Draw(gfx);
}
