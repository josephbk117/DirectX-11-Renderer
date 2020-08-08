#pragma once
#include "Bindable.h"
#include "..\Image.h"

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, Image& img, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos, bool genMipMap = true);
	Texture(Graphics& gfx, ImageHDR& img, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos, bool genMipMap = true);
	Texture(Graphics& gfx, const std::string& path, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos, bool genMipMap = true);
	void Bind(Graphics& gfx) noexcept override;
	void CustomBind(Graphics& gfx, PipelineStageSlotInfo customStageInfo) noexcept;
	void UnBind(Graphics& gfx) noexcept;
	void CustomUnBind(Graphics& gfx, PipelineStageSlotInfo customStageInfo) noexcept;
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx, const std::string& path, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos);
	static std::string GenerateUID(const std::string& path, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos);
	std::string GetUID() const noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pUAV;
	std::vector<PipelineStageSlotInfo> pipelineStageInfos;
	std::string path="";
};

