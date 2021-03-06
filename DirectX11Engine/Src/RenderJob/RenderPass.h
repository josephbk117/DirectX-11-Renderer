#pragma once
#include "../Graphics.h"
#include "RenderJob.h"
class RenderPass
{
public:
	void Accept(RenderJob job) noexcept;
	void Execute(Graphics& gfx) const;
	void Reset() noexcept;
private:
	std::vector<RenderJob> jobs;
};

