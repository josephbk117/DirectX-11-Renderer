#pragma once
#include "Bindable.h"
class StencilOperation : public Bindable
{
public:
	enum class Mode
	{
		Off,
		Write,
		Mask
	};

	StencilOperation(Graphics& gfx, Mode mode);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx, Mode mode);
	static std::string GenerateUID(Mode mode);
	std::string GetUID() const noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencil;
	Mode mode = Mode::Off;
};

