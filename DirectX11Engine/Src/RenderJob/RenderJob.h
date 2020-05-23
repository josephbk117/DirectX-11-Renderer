#pragma once

class Graphics;
class RenderStep;
class Drawable;

class RenderJob
{
public:
	RenderJob(const RenderStep* pStep, const Drawable* pDrawable);
	void Execute(Graphics& gfx) const;
private:
	const RenderStep* pStep;
	const Drawable* pDrawable;
};

