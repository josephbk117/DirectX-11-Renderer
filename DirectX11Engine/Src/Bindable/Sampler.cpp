#include "Sampler.h"
#include "BindableCodex.h"

Sampler::Sampler(Graphics& gfx)
{
	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	sd.MipLODBias = 0.0f;
	sd.MinLOD = 0.0f;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	GetDevice(gfx)->CreateSamplerState(&sd, &pSamplerState);
}

void Sampler::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());
}

std::shared_ptr<Bindable> Sampler::Resolve(Graphics& gfx)
{
	return BindableCodex::Resolve<Sampler>(gfx);
}

std::string Sampler::GenerateUID()
{
	return typeid(Sampler).name();
}

std::string Sampler::GetUID() const noexcept
{
	return GenerateUID();
}
