#pragma once
#include "..\Graphics.h"
#include <string>

class Bindable
{
public:
	virtual void Bind(Graphics& gfx) noexcept = 0;
	virtual void InitializeParentReference(const class Drawable&) noexcept {}
	virtual std::string GetUID() const noexcept;
	virtual ~Bindable() = default;
protected:
	static ID3D11Device* GetDevice(Graphics& gfx ) noexcept;
	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
};