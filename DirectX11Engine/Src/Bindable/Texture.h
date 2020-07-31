#pragma once
#include "Bindable.h"
#include "..\Image.h"

struct  PipelineStageSlotInfo
{
	PipelineStage stage = PipelineStage::PixelShader;
	UINT slot = 0;
};

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, Image& img, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos, bool genMipMap = true);
	Texture(Graphics& gfx, ImageHDR& img, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos, bool genMipMap = true);
	Texture(Graphics& gfx, const std::string& path, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos, bool genMipMap = true);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx, const std::string& path, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos);
	static std::string GenerateUID(const std::string& path, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos);
	std::string GetUID() const noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	std::vector<PipelineStageSlotInfo> pipelineStageInfos;
	std::string path="";
};

