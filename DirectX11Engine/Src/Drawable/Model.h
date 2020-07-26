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

struct TexturePipelineBind
{
	std::vector<ImageHDR*> vertexShader;
	std::vector<ImageHDR*> hullShader;
	std::vector<ImageHDR*> domainShader;
	std::vector<ImageHDR*> pixelShader;
};

class BaseMesh : public Drawable
{
public:
	virtual void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept = 0;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
protected:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Mesh : public BaseMesh
{
public:
	Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtrs);
	virtual void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept;
};

class InstancedMesh : public BaseMesh
{
public:
	InstancedMesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtrs);
	InstancedMesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtrs, ImageHDR* transformTexture);
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
	static std::unique_ptr<T> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMats, const std::filesystem::path& path, float scale, const std::string& vertexShaderPath);
	template <typename T>
	static std::unique_ptr<T> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMats, const std::filesystem::path& path, float scale, const std::string& vertexShaderPath, TexturePipelineBind& pipelineTextureOverrides);
	std::unique_ptr<Node> ParseNode(const aiNode& node);
	Node* GetSelectedNode() const noexcept;

	virtual void Draw(Graphics& gfx) = 0;
	virtual void ShowWindow(const char* windowName) noexcept = 0;
	virtual ~BaseModel() {}

protected:
	std::optional<int> selectedIndex;
	Node* pSelectedNode;
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
	Model(Graphics& gfx, const std::string& fileName, float scale = 1.0f);
	DirectX::XMMATRIX GetTransform() const noexcept;
	virtual void Draw(Graphics& gfx);
	virtual void ShowWindow(const char* windowName) noexcept;
};

class InstanceModel : public BaseModel
{
public:
	InstanceModel(Graphics& gfx, const std::string& fileName, float scale = 1.0f);
	InstanceModel(Graphics& gfx, const std::string& fileName, float scale, ImageHDR* transformTexture);
	DirectX::XMMATRIX GetTransform() const noexcept;
	virtual void Draw(Graphics& gfx);
	virtual void ShowWindow(const char* windowName) noexcept;
};