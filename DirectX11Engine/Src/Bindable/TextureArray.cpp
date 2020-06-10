#include "TextureArray.h"
#include "BindableCodex.h"

TextureArray::TextureArray(Graphics& gfx, const std::vector<std::string>& paths, unsigned int slot /*= 0*/) : paths(paths), slot(slot)
{
	pTextureViews = std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>(paths.size());

	for (int i = 0; i < paths.size(); i++)
	{
		const auto img = Image::FromFile(paths[i]);

		const bool MIPMAP_ENABLED = true;

		D3D11_TEXTURE2D_DESC td = {};
		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		td.Width = img.GetWidth();
		td.Height = img.GetHeight();
		td.MipLevels = (MIPMAP_ENABLED) ? 0 : 1; // Max number of mip levels of mipmap mode selected
		td.ArraySize = 1;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = (MIPMAP_ENABLED) ? D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET : D3D11_BIND_SHADER_RESOURCE; // D3D11_BIND_RENDER_TARGET to allow for creating mips
		td.CPUAccessFlags = 0;
		td.MiscFlags = (MIPMAP_ENABLED) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0; // If no need for mips can be set null

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;

		if (!MIPMAP_ENABLED)
		{
			D3D11_SUBRESOURCE_DATA sd;
			sd.pSysMem = img.GetData();
			sd.SysMemPitch = img.GetPixelCount() * sizeof(Image::Color);
			GetDevice(gfx)->CreateTexture2D(&td, &sd, &pTexture);
		}
		else
		{
			GetDevice(gfx)->CreateTexture2D(&td, nullptr, &pTexture);
			GetContext(gfx)->UpdateSubresource(pTexture.Get(), 0, nullptr, img.GetData(), img.GetWidth() * sizeof(Image::Color), 0);
		}


		D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
		srv.Format = td.Format;
		srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv.Texture2D.MostDetailedMip = 0;
		srv.Texture2D.MipLevels = (MIPMAP_ENABLED) ? -1 : 1; // Can set to 1 if not using mip mapping

		GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srv, &pTextureViews[i]);

		//generate mip chain on GPU
		if (MIPMAP_ENABLED)
		{
			GetContext(gfx)->GenerateMips(pTextureViews[i].Get());
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
