#include "Drawable.h"
#include "..\Bindable\Bindable.h"
#include "..\Bindable\IndexBuffer.h"
#include <cassert>
#include <typeinfo>

void Drawable::Draw(Graphics& gfx) const noexcept
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}

	gfx.DrawIndex(pIndexBuffer->GetCount());
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
