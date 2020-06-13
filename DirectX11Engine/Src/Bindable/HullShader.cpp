#include "HullShader.h"
#include "BindableCodex.h"
#include "../Utilities/EngineUtilities.h"

HullShader::HullShader(Graphics& gfx, const std::string& path)
{
	using namespace Utility;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	D3DReadFileToBlob(StringToWString(path).c_str(), &pBlob);
	GetDevice(gfx)->CreateHullShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pHullShader);
}

void HullShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->HSSetShader(pHullShader.Get(), nullptr, 0u);
}

std::shared_ptr<Bindable> HullShader::Resolve(Graphics& gfx, const std::string& path)
{
	return BindableCodex::Resolve<HullShader>(gfx, path);
}

std::string HullShader::GenerateUID(const std::string& path)
{
	using namespace std::string_literals;
	return typeid(HullShader).name() + "#"s + path;
}

std::string HullShader::GetUID() const noexcept
{
	return GenerateUID(path);
}
