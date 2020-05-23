#include "RenderJob.h"
#include "RenderStep.h"
#include "../Drawable/Drawable.h"

RenderJob::RenderJob(const RenderStep* step, const Drawable* drawable) : pDrawable(drawable), pStep(step)
{}

void RenderJob::Execute(Graphics& gfx) const
{
	pDrawable->Bind(gfx);
	pStep->Bind(gfx);
	gfx.DrawIndexed(pDrawable->GetIndexCount());
}
