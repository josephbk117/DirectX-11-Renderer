#pragma once
#include "Drawable.h"
class PhysicalSkybox : public Drawable
{
public:
	PhysicalSkybox(Graphics& gfx, const std::string& fileName);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Draw(Graphics& gfx) const noexcept override;
};

