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

void Technique::SetActivateState(bool isActive) noexcept
{
	this->isActive = isActive;
}

bool Technique::IsActive() const noexcept
{
	return isActive;
}

void Technique::Accept(TechniqueProbe& probe) noexcept
{
	probe.SetTechnique(this);
	for (auto& s : steps)
	{
		s.Accept(probe);
	}
}

void Technique::InitializeParentReferences(const Drawable& parent) noexcept
{
	for (auto& s : steps)
	{
		s.InitializeParentReferences(parent);
	}
}

const std::string& Technique::GetName() const noexcept
{
	return name;
}
