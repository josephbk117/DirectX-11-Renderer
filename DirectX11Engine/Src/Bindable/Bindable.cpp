#include "Bindable.h"

std::string Bindable::GetUID() const noexcept
{
	assert(false);
	return "";
}

ID3D11Device* Bindable::GetDevice(Graphics& gfx) noexcept
{
	return gfx.pDevice.Get();
}

ID3D11DeviceContext* Bindable::GetContext(Graphics& gfx) noexcept
{
	return gfx.pContext.Get();
}

