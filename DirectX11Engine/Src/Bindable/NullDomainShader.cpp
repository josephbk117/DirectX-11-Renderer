#include "NullDomainShader.h"
#include "BindableCodex.h"

NullDomainShader::NullDomainShader(Graphics& gfx)
{}

void NullDomainShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->DSSetShader(nullptr, nullptr, 0);
}

std::shared_ptr<Bindable> NullDomainShader::Resolve(Graphics& gfx)
{
	return BindableCodex::Resolve<NullDomainShader>(gfx);
}

std::string NullDomainShader::GenerateUID()
{
	return typeid(NullDomainShader).name();
}

std::string NullDomainShader::GetUID() const noexcept
{
	return GenerateUID();
}
