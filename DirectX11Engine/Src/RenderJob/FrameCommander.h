#pragma once

#include "RenderJob.h"
#include "RenderPass.h"
#include <array>

class FrameCommander
{
public:
	void Accept(RenderJob job, size_t target) noexcept
	{
		passes[target].Accept(job);
	}

	void Execute(Graphics& gfx) const noexcept
	{

	}

	void Reset() noexcept
	{

	}
private:
	std::array<RenderPass, 3> passes;
};