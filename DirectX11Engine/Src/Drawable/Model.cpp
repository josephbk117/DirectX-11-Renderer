#include "Model.h"

Model::Model(Graphics& gfx, const std::string& fileName, const ShaderSetPath& shaderSet, float scale /*= 1.0f*/)
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);

	for (size_t i = 0; i < pScene->mNumMeshes; ++i)
	{
		meshPtrs.push_back(ParseMesh<Mesh>(gfx, *pScene->mMeshes[i], pScene->mMaterials, fileName, scale, std::move(shaderSet)));
	}

	pRoot = ParseNode(*pScene->mRootNode);
}

DirectX::XMMATRIX Model::GetTransform() const noexcept
{
	const auto& transform = transforms.at(*selectedIndex);
	return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) * DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
}

void Model::Draw(Graphics& gfx)
{
	if (auto node = GetSelectedNode())
	{
		node->SetAppliedTransform(GetTransform());
	}
	pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}

void Model::DrawDebug(Graphics& gfx)
{
	for (const auto& mesh : meshPtrs)
	{
		mesh->DrawDebug(gfx);
	}
}

void Model::ShowWindow(const char* windowName) noexcept
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

//____________________INSTANCE MODEL___________________//

InstanceModel::InstanceModel(Graphics& gfx, const std::string& fileName, const ShaderSetPath& shaderSet, float scale /*= 1.0f*/, std::shared_ptr<Texture> tarnsformTex /*= nullptr*/)
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);

	for (size_t i = 0; i < pScene->mNumMeshes; ++i)
	{
		meshPtrs.push_back(ParseMesh<InstancedMesh>(gfx, *pScene->mMeshes[i], pScene->mMaterials, fileName, scale, std::move(shaderSet), tarnsformTex));
	}

	pRoot = ParseNode(*pScene->mRootNode);
}

DirectX::XMMATRIX InstanceModel::GetTransform() const noexcept
{
	const auto& transform = transforms.at(*selectedIndex);
	return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) * DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
}

void InstanceModel::Draw(Graphics& gfx)
{
	if (auto node = GetSelectedNode())
	{
		node->SetAppliedTransform(GetTransform());
	}
	pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}

void InstanceModel::ShowWindow(const char* windowName) noexcept
{
	windowName = windowName ? windowName : "Instanced Model";
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

//_______________________NODE IMPLEMENTATION_____________________________//

Node::Node(const std::string name, std::vector<BaseMesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept : name(name), meshPtrs(std::move(meshPtrs))
{
	DirectX::XMStoreFloat4x4(&this->transform, transform);
	DirectX::XMStoreFloat4x4(&this->appliedTransform, DirectX::XMMatrixIdentity());
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
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

void Node::ShowTree(int& nodeIndexTracked, std::optional<int>& selectedIndex, Node*& pSelectedNode) const noexcept
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

void Node::SetAppliedTransform(DirectX::FXMMATRIX _transform)
{
	DirectX::XMStoreFloat4x4(&appliedTransform, _transform);
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept
{
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}

//_______________________MESH IMPLEMENTATION_____________________________//

Mesh::Mesh(Graphics& gfx, const MeshInfo& meshInfo)
{
	boundingBoxData = meshInfo.minMaxVertexPair;

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	for (auto& pb : meshInfo.bindPtrs)
	{
		AddBind(std::move(pb));
	}

	AddBind(std::make_shared<TransformCBufferEx>(gfx, *this, 0));
}

void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}

void Mesh::DrawDebug(Graphics& gfx) const noexcept
{
	DirectX::XMFLOAT3 top1 = boundingBoxData.GetMax();
	DirectX::XMFLOAT3 top2 = { boundingBoxData.GetMin().x, top1.y, top1.z };
	DirectX::XMFLOAT3 top3 = { top1.x, top1.y,  boundingBoxData.GetMin().z };
	DirectX::XMFLOAT3 top4 = { boundingBoxData.GetMin().x, top1.y, boundingBoxData.GetMin().z };

	DirectX::XMFLOAT3 btm1 = boundingBoxData.GetMin();
	DirectX::XMFLOAT3 btm2 = { boundingBoxData.GetMax().x, btm1.y, btm1.z };
	DirectX::XMFLOAT3 btm3 = { btm1.x, btm1.y,  boundingBoxData.GetMax().z };
	DirectX::XMFLOAT3 btm4 = { boundingBoxData.GetMax().x, btm1.y, boundingBoxData.GetMax().z };


	DirectX::XMFLOAT3 midTop = boundingBoxData.GetMid();
	midTop.y = boundingBoxData.GetMax().y + 0.1f;

	DirectX::XMFLOAT3 upVec = midTop;
	upVec.y += 0.5f;

	static std::vector<DirectX::XMFLOAT3> linePoints = { midTop, upVec, top1, top2, top2, top4, top4, top3, top3, top1,
														btm1, btm2, btm2, btm4, btm4, btm3, btm3, btm1,
														top1, btm4, top2, btm3, top3, btm2, top4, btm1 };
	gfx.DrawDebugLines(linePoints, GetTransformXM());

}
//_______________________INSTANCEMESH IMPLEMENTATION_____________________________//

InstancedMesh::InstancedMesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtrs)
{
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	for (auto& pb : bindPtrs)
	{
		AddBind(std::move(pb));
	}

	AddBind(std::make_shared<TransformCBufferEx>(gfx, *this, 0));
}

InstancedMesh::InstancedMesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtrs, std::shared_ptr<Texture> tarnsformTex) : InstancedMesh(gfx, bindPtrs)
{
	AddBind(tarnsformTex);
}

void InstancedMesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::DrawInstanced(gfx, 512 * 512);
}

DirectX::XMMATRIX InstancedMesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}

//_____________________BASE MODEL IMPLEMENTATION________________________________//

template <typename T>
std::unique_ptr<T> BaseModel::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMats, const std::filesystem::path& path, float scale, const ShaderSetPath& shaderSetPath)
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

	dx::XMFLOAT3 maxExtent = { -1e10f, -1e10f, -1e10f };
	dx::XMFLOAT3 minExtent = { 1e10f, 1e10f, 1e10f };

	for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
	{
		dx::XMFLOAT3 pos = { mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale };

		dx::XMStoreFloat3(&minExtent, dx::XMVectorMin(dx::XMLoadFloat3(&pos), dx::XMLoadFloat3(&minExtent)));
		dx::XMStoreFloat3(&maxExtent, dx::XMVectorMax(dx::XMLoadFloat3(&pos), dx::XMLoadFloat3(&maxExtent)));

		vertices.push_back({ pos,
			{ mesh.mNormals[i].x,  mesh.mNormals[i].y, mesh.mNormals[i].z},
			{ mesh.mTangents[i].x, mesh.mTangents[i].y, mesh.mTangents[i].z },
			{ mesh.mBitangents[i].x, mesh.mBitangents[i].y, mesh.mBitangents[i].z },
			{ mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y} });
	}

	MeshInfo meshInfo;
	meshInfo.minMaxVertexPair.SetMinMax(minExtent, maxExtent);

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

	std::vector<std::shared_ptr<Bindable>> bindablePtrs;

	bindablePtrs.push_back(std::make_shared<VertexBuffer>(gfx, vertices));
	bindablePtrs.push_back(std::make_shared<IndexBuffer>(gfx, indices));

	auto pvs = VertexShader::Resolve(gfx, std::move(shaderSetPath.vertexShader));
	auto pvsbc = std::static_pointer_cast<VertexShader>(pvs)->GetBytecode();

	bindablePtrs.push_back(std::move(pvs));

	SamplerSettings pixelSamplerSettings
	{
		D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP
	};
	std::vector<PipelineStageSlotInfo> samplerPipelinePixelInfo;
	samplerPipelinePixelInfo.push_back({ PipelineStage::PixelShader, 0 });
	bindablePtrs.push_back(Sampler::Resolve(gfx, pixelSamplerSettings, samplerPipelinePixelInfo));

	namespace dx = DirectX;

	bindablePtrs.push_back(PixelShader::Resolve(gfx, std::move(shaderSetPath.pixelShader)));
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
			std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
			pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 0 });
			bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), pipelineStageSlotInfo));
		}
		else
		{
			Image img(1, 1);
			img.ClearData(Image::Color(255, 0, 255));
			std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
			pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 0 });
			bindablePtrs.push_back(std::make_shared<Texture>(gfx, img, pipelineStageSlotInfo));
		}

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &textFileName) == aiReturn_SUCCESS)
		{
			std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
			pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 1 });
			bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), pipelineStageSlotInfo));
		}
		else
		{
			Image img(1, 1);
			img.ClearData(Image::Color(100, 100, 100, 100));
			std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
			pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 1 });
			bindablePtrs.push_back(std::make_shared<Texture>(gfx, img, pipelineStageSlotInfo));
		}

		if (material.GetTexture(aiTextureType_NORMALS, 0, &textFileName) == aiReturn_SUCCESS)
		{
			std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
			pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 2 });
			bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), pipelineStageSlotInfo));
		}
		else
		{
			if (material.GetTexture(aiTextureType_HEIGHT, 0, &textFileName) == aiReturn_SUCCESS)
			{
				std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
				pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 2 });
				bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), pipelineStageSlotInfo));
			}
			else
			{
				Image img(1, 1);
				img.ClearData(Image::Color(128, 128, 255));
				std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
				pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 2 });
				bindablePtrs.push_back(std::make_shared<Texture>(gfx, img, pipelineStageSlotInfo));
			}
		}
		if (material.GetTexture(aiTextureType_DISPLACEMENT, 0, &textFileName) == aiReturn_SUCCESS)
		{
			std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
			pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 3 });
			bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), pipelineStageSlotInfo));
		}
		else
		{
			Image img(1, 1);
			img.ClearData(Image::Color(0, 0, 0));
			std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
			pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 3 });
			bindablePtrs.push_back(std::make_shared<Texture>(gfx, img, pipelineStageSlotInfo));
		}
	}

	meshInfo.bindPtrs = std::move(bindablePtrs);

	return std::make_unique<T>(gfx, meshInfo);
}

template <typename T>
static std::unique_ptr<T> BaseModel::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMats, const std::filesystem::path& path, float scale, const ShaderSetPath& shaderSetPath, std::shared_ptr<Texture> transformTexture)
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

	std::vector<std::shared_ptr<Bindable>> bindablePtrs;

	bindablePtrs.push_back(std::make_shared<VertexBuffer>(gfx, vertices));
	bindablePtrs.push_back(std::make_shared<IndexBuffer>(gfx, indices));

	auto pvs = VertexShader::Resolve(gfx, std::move(shaderSetPath.vertexShader));
	auto pvsbc = std::static_pointer_cast<VertexShader>(pvs)->GetBytecode();

	bindablePtrs.push_back(std::move(pvs));
	SamplerSettings pixelSamplerSettings
	{
		D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP
	};
	std::vector<PipelineStageSlotInfo> samplerPipelinePixelInfo;
	samplerPipelinePixelInfo.push_back({ PipelineStage::PixelShader, 0 });
	bindablePtrs.push_back(Sampler::Resolve(gfx, pixelSamplerSettings, samplerPipelinePixelInfo));

	namespace dx = DirectX;

	bindablePtrs.push_back(PixelShader::Resolve(gfx, std::move(shaderSetPath.pixelShader)));
	const std::vector< D3D11_INPUT_ELEMENT_DESC >ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12u,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Tangent",0,DXGI_FORMAT_R32G32B32_FLOAT,0,24u,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "BiTangent",0,DXGI_FORMAT_R32G32B32_FLOAT,0,36u,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,48u,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	bindablePtrs.push_back(std::make_shared<InputLayout>(gfx, ied, pvsbc));
	bindablePtrs.push_back(Rasterizer::Resolve(gfx, Rasterizer::RasterizerMode{ true, false }));
	bindablePtrs.push_back(BlendOperation::Resolve(gfx, true));
	bindablePtrs.push_back(transformTexture);

	if (mesh.mMaterialIndex >= 0)
	{
		using namespace std::string_literals;

		const std::string IMG_PATH = path.parent_path().string() + "\\"s + path.filename().string() + " Textures\\";

		const aiMaterial& material = *pMats[mesh.mMaterialIndex];
		aiString textFileName;


		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &textFileName) == aiReturn_SUCCESS)
		{
			std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
			pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 0 });
			bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), pipelineStageSlotInfo));
		}
		else
		{
			Image img(1, 1);
			img.ClearData(Image::Color(255, 0, 255));
			std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
			pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 0 });
			bindablePtrs.push_back(std::make_shared<Texture>(gfx, img, pipelineStageSlotInfo));
		}

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &textFileName) == aiReturn_SUCCESS)
		{
			std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
			pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 1 });
			bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), pipelineStageSlotInfo));
		}
		else
		{
			Image img(1, 1);
			img.ClearData(Image::Color(100, 100, 100, 100));
			std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
			pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 1 });
			bindablePtrs.push_back(std::make_shared<Texture>(gfx, img, pipelineStageSlotInfo));
		}

		if (material.GetTexture(aiTextureType_NORMALS, 0, &textFileName) == aiReturn_SUCCESS)
		{
			std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
			pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 2 });
			bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), pipelineStageSlotInfo));
		}
		else
		{
			if (material.GetTexture(aiTextureType_HEIGHT, 0, &textFileName) == aiReturn_SUCCESS)
			{
				std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
				pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 2 });
				bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), pipelineStageSlotInfo));
			}
			else
			{
				Image img(1, 1);
				img.ClearData(Image::Color(128, 128, 255));
				std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
				pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 2 });
				bindablePtrs.push_back(std::make_shared<Texture>(gfx, img, pipelineStageSlotInfo));
			}
		}
		if (material.GetTexture(aiTextureType_DISPLACEMENT, 0, &textFileName) == aiReturn_SUCCESS)
		{
			std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
			pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 3 });
			bindablePtrs.push_back(Texture::Resolve(gfx, IMG_PATH + textFileName.C_Str(), pipelineStageSlotInfo));
		}
		else
		{
			Image img(1, 1);
			img.ClearData(Image::Color(0, 0, 0));
			std::vector<PipelineStageSlotInfo> pipelineStageSlotInfo;
			pipelineStageSlotInfo.push_back({ PipelineStage::PixelShader, 3 });
			bindablePtrs.push_back(std::make_shared<Texture>(gfx, img, pipelineStageSlotInfo));
		}
	}

	return std::make_unique<T>(gfx, std::move(bindablePtrs));
}


std::unique_ptr<Node> BaseModel::ParseNode(const aiNode& node)
{
	namespace dx = DirectX;
	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)));

	std::vector<BaseMesh*> curMeshPtrs;
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

Node* BaseModel::GetSelectedNode() const noexcept
{
	return pSelectedNode;
}
