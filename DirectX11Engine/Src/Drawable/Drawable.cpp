#include "Drawable.h"
#include "..\Bindable\Bindable.h"
#include "..\Bindable\IndexBuffer.h"
#include "..\Bindable\VertexBuffer.h"
#include "..\Bindable\Topology.h"
#include <cassert>
#include <typeinfo>

void Drawable::AddTechnique(Technique technique) noexcept
{
	technique.InitializeParentReferences(*this);
	techniques.push_back(std::move(technique));
}

void Drawable::Submit(class FrameCommander& frame) const noexcept
{
	for (const Technique& technique : techniques)
	{
		technique.Submit(frame, *this);
	}
}

void Drawable::Bind(Graphics& gfx) const noexcept
{
	pTopo->Bind(gfx);
	pVertices->Bind(gfx);
	pIndices->Bind(gfx);
}

UINT Drawable::GetIndexCount() const noexcept
{
	return pIndices->GetCount();
}

void Drawable::Draw(Graphics& gfx) const noexcept
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}

	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::DrawInstanced(Graphics& gfx, UINT instanceCount) const noexcept
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}

	gfx.DrawIndexedInstanced(pIndexBuffer->GetCount(), instanceCount);
}

void Drawable::AddBind(std::shared_ptr<Bindable> bind) noexcept
{
	if (typeid(*bind) == typeid(IndexBuffer))
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
		pIndexBuffer = &static_cast<IndexBuffer&>(*bind);
	}

	binds.push_back(std::move(bind));
}
