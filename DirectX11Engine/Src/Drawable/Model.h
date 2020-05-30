#pragma once
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
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <optional>
#include <filesystem>

class Mesh : public Drawable
{
public:
	Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtrs)
	{
		AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		for (auto& pb : bindPtrs)
		{
			AddBind(std::move(pb));
		}

		AddBind(std::make_shared<TransformCBufferEx>(gfx, *this, 0));
	}

	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
	{
		DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
		Drawable::Draw(gfx);
	}

	DirectX::XMMATRIX GetTransformXM() const noexcept
	{
		return DirectX::XMLoadFloat4x4(&transform);
	}

private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
public:
	Node(const std::string name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept : name(name), meshPtrs(std::move(meshPtrs))
	{
		DirectX::XMStoreFloat4x4(&this->transform, transform);
		DirectX::XMStoreFloat4x4(&this->appliedTransform, DirectX::XMMatrixIdentity());
	}
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
	{
		const auto built = DirectX::XMLoadFloat4x4(&appliedTransform) * DirectX::XMLoadFloat4x4(&transform) * accumulatedTransform;
		for (const auto pm : meshPtrs)
		{
			pm->Draw(gfx, built);
		}

		for (const auto& pc : childPtrs)
		{
			pc->Draw(gfx, built);
		}
	}
	void ShowTree(int& nodeIndexTracked, std::optional<int>& selectedIndex, Node*& pSelectedNode) const noexcept
	{
		const int currentNodeIndex = nodeIndexTracked;
		nodeIndexTracked++;

		const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
			| ((currentNodeIndex == selectedIndex.value_or(-1)) ? ImGuiTreeNodeFlags_Selected : 0)
			| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);


		const auto expanded = ImGui::TreeNodeEx(
			(void*)(intptr_t)currentNodeIndex, node_flags, name.c_str()
		);


		if (ImGui::IsItemClicked())
		{
			selectedIndex = currentNodeIndex;
			pSelectedNode = const_cast<Node*>(this);
		}

		if (expanded)
		{
			for (const auto& pChild : childPtrs)
			{
				pChild->ShowTree(nodeIndexTracked, selectedIndex, pSelectedNode);
			}
			ImGui::TreePop();
		}
	}

	void SetAppliedTransform(DirectX::FXMMATRIX _transform)
	{
		DirectX::XMStoreFloat4x4(&appliedTransform, _transform);
	}
private:

	void AddChild(std::unique_ptr<Node> pChild) noexcept
	{
		assert(pChild);
		childPtrs.push_back(std::move(pChild));
	}

	std::string name;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
};

class Model
{
public:
	Model(Graphics& gfx, const std::string& fileName, float scale = 1.0f)
	{
		Assimp::Importer imp;
		const auto pScene = imp.ReadFile(fileName, aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ConvertToLeftHanded |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace);

		for (size_t i = 0; i < pScene->mNumMeshes; ++i)
		{
			meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, fileName, scale));
		}

		pRoot = ParseNode(*pScene->mRootNode);
	}

	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMats, const std::filesystem::path& path, float scale)
	{
		namespace dx = DirectX;
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 norm;
			dx::XMFLOAT3 tan;
			dx::XMFLOAT3 biTan;
			dx::XMFLOAT2 uv;
		};

		std::vector<Vertex> vertices;
		vertices.reserve(mesh.mNumVertices);

		for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
		{
			vertices.push_back({ { mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale },
				{ mesh.mNormals[i].x,  mesh.mNormals[i].y, mesh.mNormals[i].z},
				{ mesh.mTangents[i].x, mesh.mTangents[i].y, mesh.mTangents[i].z },
				{ mesh.mBitangents[i].x, mesh.mBitangents[i].y, mesh.mBitangents[i].z },
				{ mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y} });
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

		std::vector<std::shared_ptr<Bindable>> bindablePtrs;

		bindablePtrs.push_back(std::make_shared<VertexBuffer>(gfx, vertices));
		bindablePtrs.push_back(std::make_shared<IndexBuffer>(gfx, indices));

		auto pvs = VertexShader::Resolve(gfx, "Shaders\\VertexShader.cso");
		auto pvsbc = std::static_pointer_cast<VertexShader>(pvs)->GetBytecode();

		bindablePtrs.push_back(std::move(pvs));
		bindablePtrs.push_back(Sampler::Resolve(gfx));

		namespace dx = DirectX;

		bindablePtrs.push_back(PixelShader::Resolve(gfx, "Shaders\\PixelShader.cso"));
		const std::vector< D3D11_INPUT_ELEMENT_DESC >ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12u,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Tangent",0,DXGI_FORMAT_R32G32B32_FLOAT,0,24u,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "BiTangent",0,DXGI_FORMAT_R32G32B32_FLOAT,0,36u,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,48u,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		bindablePtrs.push_back(std::make_shared<InputLayout>(gfx, ied, pvsbc));

		bindablePtrs.push_back(Rasterizer::Resolve(gfx, Rasterizer::RasterizerMode{ false, false }));

		if (mesh.mMaterialIndex >= 0)
		{
			using namespace std::string_literals;

			const std::string IMG_PATH = path.parent_path().string() + "\\"s + path.filename().string() + " Textures\\";

			const aiMaterial& material = *pMats[mesh.mMaterialIndex];
			aiString textFileName;


			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &textFileName) == aiReturn_SUCCESS)
			{
				bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), 0));
			}
			else
			{
				Image img(1, 1);
				img.ClearData(Image::Color(255, 0, 255));
				bindablePtrs.push_back(std::make_shared<Texture>(gfx, img));
			}

			if (material.GetTexture(aiTextureType_SPECULAR, 0, &textFileName) == aiReturn_SUCCESS)
			{
				bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), 1));
			}
			else
			{
				//float shininess = 60.0f;
				//material.Get(AI_MATKEY_SHININESS, shininess);
				Image img(1, 1);
				img.ClearData(Image::Color(100, 100, 100, 100));
				bindablePtrs.push_back(std::make_shared<Texture>(gfx, img, 1));
			}

			if (material.GetTexture(aiTextureType_NORMALS, 0, &textFileName) == aiReturn_SUCCESS)
			{
				bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), 2));
			}
			else
			{
				if (material.GetTexture(aiTextureType_HEIGHT, 0, &textFileName) == aiReturn_SUCCESS)
				{
					bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), 2));
				}
				else
				{
					Image img(1, 1);
					img.ClearData(Image::Color(128, 128, 255));
					bindablePtrs.push_back(std::make_shared<Texture>(gfx, img, 2));
				}
			}
			if (material.GetTexture(aiTextureType_DISPLACEMENT, 0, &textFileName) == aiReturn_SUCCESS)
			{
				bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), 3));
			}
			else
			{
				Image img(1, 1);
				img.ClearData(Image::Color(0, 0, 0));
				bindablePtrs.push_back(std::make_shared<Texture>(gfx, img, 3));
			}
		}

		return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
	}

	std::unique_ptr<Node> ParseNode(const aiNode& node)
	{
		namespace dx = DirectX;
		const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)));

		std::vector<Mesh*> curMeshPtrs;
		curMeshPtrs.reserve(node.mNumMeshes);

		for (size_t i = 0; i < node.mNumMeshes; ++i)
		{
			const auto meshIdx = node.mMeshes[i];
			curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
		}

		auto pNode = std::make_unique<Node>(node.mName.C_Str(), std::move(curMeshPtrs), transform);
		for (size_t i = 0; i < node.mNumChildren; ++i)
		{
			pNode->AddChild(ParseNode(*node.mChildren[i]));
		}

		return pNode;
	}

	DirectX::XMMATRIX GetTransform() const noexcept
	{
		const auto& transform = transforms.at(*selectedIndex);
		return
			DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}

	void Draw(Graphics& gfx)
	{
		if (auto node = GetSelectedNode())
		{
			node->SetAppliedTransform(GetTransform());
		}
		pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
	}

	void ShowWindow(const char* windowName) noexcept
	{
		windowName = windowName ? windowName : "Model";
		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true);
			int index = 0;
			pRoot->ShowTree(index, selectedIndex, pSelectedNode);

			ImGui::NextColumn();

			if (pSelectedNode)
			{
				auto& transform = transforms[*selectedIndex];

				ImGui::Text("Orientation");

				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);

				ImGui::Text("Position");

				ImGui::SliderFloat("X", &transform.x, -1000.0f, 1000.0f);
				ImGui::SliderFloat("Y", &transform.y, -1000.0f, 1000.0f);
				ImGui::SliderFloat("Z", &transform.z, -1000.0f, 1000.0f);
			}
		}
		ImGui::End();
	}

	Node* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}

private:
	std::optional<int> selectedIndex;
	Node* pSelectedNode;
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	struct TransformParameters
	{
		float roll, yaw, pitch;
		float x, y, z;
	};
	std::unordered_map<int, TransformParameters> transforms;
};