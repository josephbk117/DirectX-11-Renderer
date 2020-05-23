#include "Technique.h"
#include "FrameCommander.h"
#include "../Drawable/Drawable.h"

void Technique::Submit(FrameCommander& frame, const Drawable& drawable) const noexcept
{
	if (isActive)
	{
		for (const auto& step : steps)
		{
			step.Submit(frame, drawable);
		}
	}
}

void Technique::AddStep(RenderStep step) noexcept
{
	steps.push_back(std::move(step));
}

void Technique::Activate() noexcept
{
	isActive = true;
}

void Technique::Deactivate() noexcept
{
	isActive = false;
}

void Technique::InitializeParentReferences(const Drawable& parent) noexcept
{
	for (auto& s : steps)
	{
		s.InitializeParentReferences(parent);
	}
}
