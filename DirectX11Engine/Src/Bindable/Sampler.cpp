#include "Sampler.h"
#include "BindableCodex.h"

Sampler::Sampler(Graphics& gfx, const SamplerSettings& settings, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos) : settings(settings), pipelineStageInfos(pipelineStageInfos)
{
	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = settings.filter;
	sd.AddressU = settings.u;
	sd.AddressV = settings.v;
	sd.AddressW = settings.w;
	sd.MaxAnisotropy = (settings.filter == D3D11_FILTER_ANISOTROPIC) ? D3D11_REQ_MAXANISOTROPY : 0;
	sd.MipLODBias = 0.0f;
	sd.MinLOD = 0.0f;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	GetDevice(gfx)->CreateSamplerState(&sd, &pSamplerState);
}

void Sampler::Bind(Graphics& gfx) noexcept
{
	for (const PipelineStageSlotInfo& psi : pipelineStageInfos)
	{
		switch (psi.stage)
		{
		case PipelineStage::VertexShader:
			GetContext(gfx)->VSSetSamplers(psi.slot, 1, pSamplerState.GetAddressOf());
			break;
		case PipelineStage::HullShader:
			GetContext(gfx)->HSSetSamplers(psi.slot, 1, pSamplerState.GetAddressOf());
			break;
		case PipelineStage::DomainShader:
			GetContext(gfx)->DSSetSamplers(psi.slot, 1, pSamplerState.GetAddressOf());
			break;
		case PipelineStage::PixelShader:
			GetContext(gfx)->PSSetSamplers(psi.slot, 1, pSamplerState.GetAddressOf());
			break;
		case PipelineStage::ComputeShader:
			GetContext(gfx)->CSSetSamplers(psi.slot, 1, pSamplerState.GetAddressOf());
			break;
		default:
			GetContext(gfx)->PSSetSamplers(psi.slot, 1, pSamplerState.GetAddressOf());
			break;
		}
	}
}

std::shared_ptr<Bindable> Sampler::Resolve(Graphics& gfx, const SamplerSettings& settings, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos)
{
	return BindableCodex::Resolve<Sampler>(gfx, settings, pipelineStageInfos);
}

std::string Sampler::GenerateUID(const SamplerSettings& settings, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos)
{
	using namespace std::string_literals;
	std::string completeStr;
	for (const PipelineStageSlotInfo& psi : pipelineStageInfos)
	{
		completeStr += std::to_string(psi.slot) + "#"s + std::to_string(static_cast<int>(psi.stage));
	}
	return typeid(Sampler).name() + std::to_string(settings.filter) + std::to_string(settings.u) + std::to_string(settings.v) + std::to_string(settings.w) + completeStr;
}

std::string Sampler::GetUID() const noexcept
{
	return GenerateUID(settings, pipelineStageInfos);
}
