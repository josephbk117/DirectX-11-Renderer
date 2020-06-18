#pragma once
#include "..\Graphics.h"
#include "..\RenderJob\TechniqueProbe.h"
#include <string>

enum class PipelineStage
{
	VertexShader,
	HullShader,
	DomainShader,
	PixelShader
};

class Bindable
{
public:
	virtual void Bind(Graphics& gfx) noexcept = 0;
	virtual void InitializeParentReference(const class Drawable&) noexcept {}
	virtual void Accept(TechniqueProbe& probe) {};
	virtual std::string GetUID() const noexcept;
	virtual ~Bindable() = default;
protected:
	static ID3D11Device* GetDevice(Graphics& gfx ) noexcept;
	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
};