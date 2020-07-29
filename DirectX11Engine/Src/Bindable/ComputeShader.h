#pragma once
#include "Bindable.h"

class ComputeShader : public Bindable
{
public:
	ComputeShader(Graphics& gfx, const std::string& path);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx, const std::string& path);
	static std::string GenerateUID(const std::string& path);
	std::string GetUID() const noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> pComputeShader;
	std::string path;
};

