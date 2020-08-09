#include "TextureArray.h"
#include "BindableCodex.h"
#include "../Utilities/DDSTextureLoader/DDSTextureLoader.h"
#include "../Utilities/WICTextureLoader/WICTextureLoader.h"
#include "../Utilities/EngineUtilities.h"

TextureArray::TextureArray(Graphics& gfx, const std::vector<std::string>& paths, unsigned int slot /*= 0*/, bool genMips /*=true*/) : paths(paths), slot(slot)
{
	pTextureViews = std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>(paths.size());

	for (int i = 0; i < paths.size(); i++)
	{
		HRESULT hr;

		if (paths[i].substr(paths[i].size() - 4, 4) == ".dds")
		{
			hr = DirectX::CreateDDSTextureFromFile(GetDevice(gfx), Utility::StringToWString(paths[i]).c_str(), nullptr, pTextureViews[i].GetAddressOf());
		}
		else
		{
			if (genMips)
			{
				hr = DirectX::CreateWICTextureFromFile(GetDevice(gfx), GetContext(gfx), Utility::StringToWString(paths[i]).c_str(), nullptr, pTextureViews[i].GetAddressOf());
			}
			else
			{
				hr = DirectX::CreateWICTextureFromFile(GetDevice(gfx), Utility::StringToWString(paths[i]).c_str(), nullptr, pTextureViews[i].GetAddressOf());
			}
		}

		if (FAILED(hr))
		{
			using namespace std::string_literals;
			OutputDebugString(("File load failed, image : "s + paths[i] + "\n").c_str());
		}
	}
}

void TextureArray::Bind(Graphics& gfx) noexcept
{
	std::vector<ID3D11ShaderResourceView*> adresses;

	for (int i = 0; i < pTextureViews.size(); i++)
	{
		adresses.push_back(pTextureViews[i].GetAddressOf()[0]);
	}

	GetContext(gfx)->PSSetShaderResources(slot, pTextureViews.size(), adresses.data());
}

std::shared_ptr<Bindable> TextureArray::Resolve(Graphics& gfx, const std::vector<std::string>& paths, UINT slot)
{
	return BindableCodex::Resolve<TextureArray>(gfx, paths, slot);
}

std::string TextureArray::GenerateUID(const std::vector<std::string>& paths, UINT slot)
{
	using namespace std::string_literals;

	std::string combinedPath;
	for (int i = 0; i < paths.size(); i++)
	{
		combinedPath += paths[i];
	}

	return typeid(TextureArray).name() + "#"s + combinedPath;
}

std::string TextureArray::GetUID() const noexcept
{
	return GenerateUID(paths, slot);
}
