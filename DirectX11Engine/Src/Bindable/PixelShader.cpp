#include "PixelShader.h"
#include "BindableCodex.h"
#include "../Utilities/EngineUtilities.h"

PixelShader::PixelShader(Graphics& gfx, const std::string& path) : path(path)
{
	using namespace Utility;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	D3DReadFileToBlob(StringToWString(path).c_str(), &pBlob);
	GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
}

void PixelShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0u);
}

std::shared_ptr<Bindable> PixelShader::Resolve(Graphics& gfx, const std::string& path)
{
	return BindableCodex::Resolve<PixelShader>(gfx, path);
}

std::string PixelShader::GenerateUID(const std::string& path)
{
	using namespace std::string_literals;
	return typeid(PixelShader).name() + "#"s + path;
}

std::string PixelShader::GetUID() const noexcept
{
	return GenerateUID(path);
}
