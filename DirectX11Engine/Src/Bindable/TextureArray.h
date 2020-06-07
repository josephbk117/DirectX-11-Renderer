#pragma once
#include "Texture.h"

class TextureArray : public Bindable
{
public:
	TextureArray(Graphics& gfx, const std::vector<std::string>& paths, unsigned int slot = 0);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx, const std::vector<std::string>& paths, UINT slot);
	static std::string GenerateUID(const std::vector<std::string>& paths, UINT slot);
	std::string GetUID() const noexcept override;
protected:
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> pTextureViews;
	unsigned int slot = 0;
	std::vector<std::string> paths;
};

