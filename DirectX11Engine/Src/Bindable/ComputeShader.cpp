#include "ComputeShader.h"
#include "BindableCodex.h"
#include "../Utilities/EngineUtilities.h"

ComputeShader::ComputeShader(Graphics& gfx, const std::string& path) : path(path)
{
	using namespace Utility;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	D3DReadFileToBlob(StringToWString(path).c_str(), &pBlob);
	GetDevice(gfx)->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pComputeShader);
}

void ComputeShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->CSSetShader(pComputeShader.Get(), nullptr, 0u);
}

std::shared_ptr<Bindable> ComputeShader::Resolve(Graphics& gfx, const std::string& path)
{
	return BindableCodex::Resolve<ComputeShader>(gfx, path);
}

std::string ComputeShader::GenerateUID(const std::string& path)
{
	using namespace std::string_literals;
	return typeid(ComputeShader).name() + "#"s + path;
}

std::string ComputeShader::GetUID() const noexcept
{
	return GenerateUID(path);
}
