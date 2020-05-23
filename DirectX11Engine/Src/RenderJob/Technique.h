#pragma once
#include "RenderStep.h"
#include <vector>

class Drawable;
class FrameCommander;

class Technique
{
public:
	void Submit(FrameCommander& frame, const Drawable& drawable) const noexcept;
	void AddStep(RenderStep step) noexcept;
	void Activate() noexcept;
	void Deactivate() noexcept;
	void InitializeParentReferences(const Drawable& parent) noexcept;
private:
	bool isActive = true;
	std::vector<RenderStep> steps;
};

