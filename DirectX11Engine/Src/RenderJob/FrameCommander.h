#pragma once

#include "RenderJob.h"
#include "RenderPass.h"
#include <array>
#include "../Bindable/StencilOperation.h"

class FrameCommander
{
public:
	void Accept(RenderJob job, size_t target) noexcept
	{
		passes[target].Accept(job);
	}

	void Execute(Graphics& gfx) const noexcept
	{
		StencilOperation::Resolve(gfx, StencilOperation::Mode::Off)->Bind(gfx);
		passes[0].Execute(gfx);

		StencilOperation::Resolve(gfx, StencilOperation::Mode::Write)->Bind(gfx);
		// NULL PIX
		passes[1].Execute(gfx);

		StencilOperation::Resolve(gfx, StencilOperation::Mode::Mask)->Bind(gfx);
		passes[2].Execute(gfx);
	}

	void Reset() noexcept
	{

	}
private:
	std::array<RenderPass, 3> passes;
};