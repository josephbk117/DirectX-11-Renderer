#include "RenderStep.h"
#include "FrameCommander.h"
#include "../Drawable/Drawable.h"
#include "../Bindable/Bindable.h"

void RenderStep::Submit(FrameCommander& frame, const Drawable& drawable) const
{
	frame.Accept(RenderJob{ this, &drawable }, targetPass);
}

void RenderStep::AddBindable(std::shared_ptr<Bindable> pBind) noexcept
{
	bindables.push_back(std::move(pBind));
}

void RenderStep::Bind(Graphics& gfx) const
{
	for (const auto& b : bindables)
	{
		b->Bind(gfx);
	}
}

void RenderStep::Accept(TechniqueProbe& probe)
{
	probe.SetStep(this);
	for (auto& b : bindables)
	{
		b->Accept(probe);
	}
}

void RenderStep::InitializeParentReferences(const Drawable& parent) noexcept
{
	for (auto& b : bindables)
	{
		b->InitializeParentReference(parent);
	}
}
