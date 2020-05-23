#include "RenderPass.h"

void RenderPass::Accept(RenderJob job) noexcept
{
	jobs.push_back(job);
}

void RenderPass::Execute(Graphics& gfx)
{
	for (const auto& j : jobs)
	{
		j.Execute(gfx);
	}
}

void RenderPass::Reset() noexcept
{
	jobs.clear();
}
