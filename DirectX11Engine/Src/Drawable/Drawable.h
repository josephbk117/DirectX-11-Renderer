#pragma once
#include "..\Graphics.h"
#include "..\RenderJob\Technique.h"
#include <DirectXMath.h>

class Bindable;
class IndexBuffer;
class VertexBuffer;
class Topology;

class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	void AddTechnique(Technique technique) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Submit(class FrameCommander& frame) const noexcept;
	void Bind(Graphics& gfx) const noexcept;
	UINT GetIndexCount() const noexcept;
	virtual void Draw(Graphics& gfx) const noexcept; // Mark for delete
	virtual ~Drawable() = default;
protected:
	void AddBind(std::shared_ptr<Bindable> bind) noexcept;
private:
	const class IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bindable>> binds; // Mark for delete
	std::shared_ptr<IndexBuffer> pIndices;
	std::shared_ptr<VertexBuffer> pVertices;
	std::shared_ptr<Topology> pTopo;
	std::vector<Technique> techniques;
};