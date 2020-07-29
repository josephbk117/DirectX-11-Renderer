#include "DomainShader.h"
#include "BindableCodex.h"
#include "../Utilities/EngineUtilities.h"

DomainShader::DomainShader(Graphics& gfx, const std::string& path) : path(path)
{
	using namespace Utility;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	D3DReadFileToBlob(StringToWString(path).c_str(), &pBlob);
	GetDevice(gfx)->CreateDomainShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pDomainShader);
}

void DomainShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->DSSetShader(pDomainShader.Get(), nullptr, 0u);
}

std::shared_ptr<Bindable> DomainShader::Resolve(Graphics& gfx, const std::string& path)
{
	return BindableCodex::Resolve<DomainShader>(gfx, path);
}

std::string DomainShader::GenerateUID(const std::string& path)
{
	using namespace std::string_literals;
	return typeid(DomainShader).name() + "#"s + path;
}

std::string DomainShader::GetUID() const noexcept
{
	return GenerateUID(path);
}
