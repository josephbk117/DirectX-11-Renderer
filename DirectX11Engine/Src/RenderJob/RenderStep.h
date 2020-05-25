#pragma once
#include "../Bindable/Bindable.h"
#include "TechniqueProbe.h"

class FrameCommander;
class Drawable;

class RenderStep
{
public:
	void Submit(FrameCommander& frame, const Drawable& drawable) const;
	void AddBindable(std::shared_ptr<Bindable> pBind) noexcept;
	void Bind(Graphics& gfx) const;
	void Accept(TechniqueProbe& probe);
	void InitializeParentReferences(const Drawable& parent) noexcept;
private:
	size_t targetPass;
	std::vector<std::shared_ptr<Bindable>> bindables;
};

