#pragma once
#include "..\Graphics.h"
#include "..\RenderJob\TechniqueProbe.h"
#include <string>

enum class PipelineStage : unsigned char
{
	VertexShader,
	HullShader,
	DomainShader,
	PixelShader,
	ComputeShader
};

enum class BufferUsage
{
	Immutable = D3D11_USAGE::D3D11_USAGE_IMMUTABLE, // cannot be updated, uses immutable create flag, Used for meshes that will be never modified
	LongLived = D3D11_USAGE::D3D11_USAGE_DEFAULT, // can be updated with UpdateSubResource, uses default create flag, used for meshes like characters
	Transient = D3D11_USAGE::D3D11_USAGE_DYNAMIC, // can be updated with CTransientBuffer, uses dynamic create flag, used for UI/Text
	Temporary = D3D11_USAGE::D3D11_USAGE_DYNAMIC, // can be updated with map with no_overwrite, uses dynamic create flag, used for particles
	Constant = D3D11_USAGE::D3D11_USAGE_DYNAMIC,  // can be updated with map with discard, uses dynamic create flag, used for material properties
	Staging = D3D11_USAGE::D3D11_USAGE_DYNAMIC,   // used for setting up buffer data copies, allows data transfer from GPU to CPU
};

struct  PipelineStageSlotInfo
{
	PipelineStage stage = PipelineStage::PixelShader;
	UINT slot = 0;
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