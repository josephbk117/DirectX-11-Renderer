#include "NullHullShader.h"
#include "BindableCodex.h"

NullHullShader::NullHullShader(Graphics& gfx)
{}

void NullHullShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->HSSetShader(nullptr, nullptr, 0);
}

std::shared_ptr<Bindable> NullHullShader::Resolve(Graphics& gfx)
{
	return BindableCodex::Resolve<NullHullShader>(gfx);
}

std::string NullHullShader::GenerateUID()
{
	return typeid(NullHullShader).name();
}

std::string NullHullShader::GetUID() const noexcept
{
	return GenerateUID();
}
