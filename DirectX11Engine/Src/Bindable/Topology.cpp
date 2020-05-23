#include "Topology.h"
#include "BindableCodex.h"

Topology::Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology) : topology(topology)
{}

void Topology::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->IASetPrimitiveTopology(topology);
}

std::shared_ptr<Bindable> Topology::Resolve(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	return BindableCodex::Resolve<Topology>(gfx, topology);
}

std::string Topology::GenerateUID(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	using namespace std::string_literals;
	return typeid(Topology).name() + "#"s + std::to_string(topology);
}

std::string Topology::GetUID() const noexcept
{
	return GenerateUID(topology);
}
