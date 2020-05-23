#pragma once
#include "Bindable.h"

class Topology : public Bindable
{
public:
	Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology);
	static std::string GenerateUID(D3D11_PRIMITIVE_TOPOLOGY topology);
	std::string GetUID() const noexcept override;
private:
	D3D11_PRIMITIVE_TOPOLOGY topology;
};

