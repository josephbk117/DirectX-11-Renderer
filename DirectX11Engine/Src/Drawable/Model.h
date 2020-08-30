#pragma once
#include "../Bindable/BindableCommon.h"
#include "../Bindable/TransformCBufferEx.h"
#include "../Utilities/BoundingVolume/BoundingBox.h"
#include "../Imgui/imgui.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <optional>
#include <filesystem>

struct TexturePipelineBind
{
	std::vector<ImageHDR*> vertexShader;
	std::vector<ImageHDR*> hullShader;
	std::vector<ImageHDR*> domainShader;
	std::vector<ImageHDR*> pixelShader;
};

struct ShaderSetPath
{
	std::string vertexShader;
	std::string hullShader;
	std::string domainShader;
	std::string pixelShader;
};

struct MeshInfo
{
	std::vector<std::shared_ptr<Bindable>> bindPtrs;
	MinMaxVertexPair minMaxVertexPair;
};

class BaseMesh : public Drawable
{
public:
	virtual void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept = 0;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	MinMaxVertexPair boundingBoxData;
protected:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Mesh : public BaseMesh
{
public:
	Mesh(Graphics& gfx, const MeshInfo& meshInfo);
	virtual void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept;
	void DrawDebug(Graphics& gfx) const noexcept override;
};

class InstancedMesh : public BaseMesh
{
public:
	InstancedMesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtrs);
	InstancedMesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtrs, std::shared_ptr<Texture> tarnsformTex);
	virtual void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept;
};

class Node
{
	friend class BaseModel;
public:

	Node(const std::string name, std::vector<BaseMesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept;
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
	void ShowTree(int& nodeIndexTracked, std::optional<int>& selectedIndex, Node*& pSelectedNode) const noexcept;
	void SetAppliedTransform(DirectX::FXMMATRIX _transform);

private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept;

	std::string name;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<BaseMesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
};

class BaseModel
{
public:
	template <typename T>
	static std::unique_ptr<T> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMats, const std::filesystem::path& path, float scale, const ShaderSetPath& shaderSetPath);
	template <typename T>
	static std::unique_ptr<T> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMats, const std::filesystem::path& path, float scale, const ShaderSetPath& shaderSetPath, std::shared_ptr<Texture> transformTexture);
	std::unique_ptr<Node> ParseNode(const aiNode& node);
	Node* GetSelectedNode() const noexcept;

	virtual void Draw(Graphics& gfx) = 0;
	virtual void DrawDebug(Graphics& gfx) {};
	virtual void ShowWindow(const char* windowName) noexcept = 0;
	virtual ~BaseModel() {}

protected:
	std::optional<int> selectedIndex;
	Node* pSelectedNode = nullptr;
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<BaseMesh>> meshPtrs;
	struct TransformParameters
	{
		float roll, yaw, pitch;
		float x, y, z;
	};
	std::unordered_map<int, TransformParameters> transforms;
};


class Model : public BaseModel
{
public:
	Model(Graphics& gfx, const std::string& fileName, const ShaderSetPath& shaderSet, float scale = 1.0f);
	DirectX::XMMATRIX GetTransform() const noexcept;
	virtual void Draw(Graphics& gfx);
	virtual void DrawDebug(Graphics& gfx) override;
	virtual void ShowWindow(const char* windowName) noexcept;
};

class InstanceModel : public BaseModel
{
public:
	InstanceModel(Graphics& gfx, const std::string& fileName, const ShaderSetPath& shaderSet, float scale = 1.0f, std::shared_ptr<Texture> tarnsformTex = nullptr);
	DirectX::XMMATRIX GetTransform() const noexcept;
	virtual void Draw(Graphics& gfx);
	virtual void ShowWindow(const char* windowName) noexcept;
};