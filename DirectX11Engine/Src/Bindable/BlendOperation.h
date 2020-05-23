#pragma once
#include "Bindable.h"
class BlendOperation : public Bindable
{
public:
	BlendOperation(Graphics& gfx, bool blendingEnabled);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx, bool blendingEnabled);
	static std::string GenerateUID(bool blendingEnabled);
	std::string GetUID() const noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendOp;
	bool isBlendingEnabled = false;
};

