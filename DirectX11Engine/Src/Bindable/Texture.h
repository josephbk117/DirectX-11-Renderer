#pragma once
#include "Bindable.h"
#include "..\Image.h"

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, Image& img, unsigned int slot = 0, PipelineStage stage = PipelineStage::PixelShader);
	Texture(Graphics& gfx, const std::string& path, unsigned int slot = 0, PipelineStage stage = PipelineStage::PixelShader);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx, const std::string& path, UINT slot);
	static std::string GenerateUID(const std::string& path, UINT slot);
	std::string GetUID() const noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	unsigned int slot = 0;
	PipelineStage stage = PipelineStage::PixelShader;
	std::string path="";
};

