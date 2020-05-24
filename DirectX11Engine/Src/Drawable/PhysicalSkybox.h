#pragma once
#include "Drawable.h"
#include "../Bindable/ConstantBuffers.h"

class PhysicalSkybox : public Drawable
{
public:
	PhysicalSkybox(Graphics& gfx, const std::string& fileName);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Draw(Graphics& gfx) const noexcept override;
	void Update(Graphics& gfx) noexcept;
	void ShowWindow(const char* windowName) noexcept;
private:
	struct SkyInfo
	{
		SkyInfo() {}

		float rayleighDensity = 7994.0f;
		float mieDensity = 1200.0;
		float height = 1.0f;
		float sunPower = 20.0f;
		DirectX::XMFLOAT3A sunDir = { 0.0f, 1.0f, 0.0f };

	} 
	skyInfo;

	std::shared_ptr<PixelConstantBuffer<SkyInfo>> pSkyInfoBuffer = nullptr;
};
