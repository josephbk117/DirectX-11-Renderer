#pragma once
#include "RenderStep.h"
#include "TechniqueProbe.h"
#include <vector>


class Drawable;
class FrameCommander;

class Technique
{
public:
	void Submit(FrameCommander& frame, const Drawable& drawable) const noexcept;
	void AddStep(RenderStep step) noexcept;
	void SetActivateState(bool isActive) noexcept;
	bool IsActive() const noexcept;
	void Accept(TechniqueProbe& probe) noexcept;
	void InitializeParentReferences(const Drawable& parent) noexcept;
	const std::string& GetName() const noexcept;
private:
	bool isActive = true;
	std::vector<RenderStep> steps;
	std::string name = "Unnamed Technique";
};

