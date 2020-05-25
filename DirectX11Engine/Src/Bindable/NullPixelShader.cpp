#include "NullPixelShader.h"
#include "BindableCodex.h"

NullPixelShader::NullPixelShader(Graphics& gfx)
{}

void NullPixelShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShader(nullptr, nullptr, 0);
}

std::shared_ptr<Bindable> NullPixelShader::Resolve(Graphics& gfx)
{
	return BindableCodex::Resolve<NullPixelShader>(gfx);
}

std::string NullPixelShader::GenerateUID()
{
	return typeid(NullPixelShader).name();
}

std::string NullPixelShader::GetUID() const noexcept
{
	return GenerateUID();
}
