#pragma once
#include "Bindable.h"

struct SamplerSettings
{
	D3D11_FILTER filter = D3D11_FILTER_ANISOTROPIC;
	D3D11_TEXTURE_ADDRESS_MODE u = D3D11_TEXTURE_ADDRESS_WRAP;
	D3D11_TEXTURE_ADDRESS_MODE v = D3D11_TEXTURE_ADDRESS_WRAP;
	D3D11_TEXTURE_ADDRESS_MODE w = D3D11_TEXTURE_ADDRESS_WRAP;
};

class Sampler : public Bindable
{
public:
	Sampler(Graphics& gfx, const SamplerSettings& settings, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx, const SamplerSettings& settings, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos);
	static std::string GenerateUID(const SamplerSettings& settings, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos);
	std::string GetUID() const noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
private:
	std::vector<PipelineStageSlotInfo> pipelineStageInfos;
	SamplerSettings settings;
};

