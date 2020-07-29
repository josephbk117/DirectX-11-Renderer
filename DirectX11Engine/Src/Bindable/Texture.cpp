#include "Texture.h"
#include "../Image.h"
#include "BindableCodex.h"

Texture::Texture(Graphics& gfx, Image& img, unsigned int slot, PipelineStage stage /*= PipelineStage::PixelShader*/) : slot(slot), stage(stage)
{
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

	GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srv, &pTextureView);

	//generate mip chain on GPU
	if (MIPMAP_ENABLED)
	{
		GetContext(gfx)->GenerateMips(pTextureView.Get());
	}
}

Texture::Texture(Graphics& gfx, ImageHDR& img, unsigned int slot /*= 0*/, PipelineStage stage /*= PipelineStage::PixelShader*/) : slot(slot), stage(stage)
{
	const bool MIPMAP_ENABLED = true;

	D3D11_TEXTURE2D_DESC td = {};
	td.Format = img.format;
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
		sd.SysMemPitch = img.GetPixelCount() * sizeof(ImageHDR::ColorFloat);
		GetDevice(gfx)->CreateTexture2D(&td, &sd, &pTexture);
	}
	else
	{
		GetDevice(gfx)->CreateTexture2D(&td, nullptr, &pTexture);
		GetContext(gfx)->UpdateSubresource(pTexture.Get(), 0, nullptr, img.GetData(), img.GetWidth() * sizeof(ImageHDR::ColorFloat), 0);
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Format = td.Format;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MostDetailedMip = 0;
	srv.Texture2D.MipLevels = (MIPMAP_ENABLED) ? -1 : 1; // Can set to 1 if not using mip mapping

	GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srv, &pTextureView);

	//generate mip chain on GPU
	if (MIPMAP_ENABLED)
	{
		GetContext(gfx)->GenerateMips(pTextureView.Get());
	}
}

Texture::Texture(Graphics& gfx, const std::string& path, unsigned int slot /*= 0*/, PipelineStage stage /*= PipelineStage::PixelShader*/) : path(path), slot(slot), stage(stage)
{
	const auto img = Image::FromFile(path);

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

	GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srv, &pTextureView);

	//generate mip chain on GPU
	if (MIPMAP_ENABLED)
	{
		GetContext(gfx)->GenerateMips(pTextureView.Get());
	}
}


void Texture::Bind(Graphics& gfx) noexcept
{
	switch (stage)
	{
	case PipelineStage::VertexShader:
		GetContext(gfx)->VSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
		break;
	case PipelineStage::HullShader:
		GetContext(gfx)->HSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
		break;
	case PipelineStage::DomainShader:
		GetContext(gfx)->DSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
		break;
	case PipelineStage::PixelShader:
		GetContext(gfx)->PSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
		break;
	case PipelineStage::ComputeShader:
		GetContext(gfx)->CSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
		break;
	default:
		GetContext(gfx)->PSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
		break;
	}
}

std::shared_ptr<Bindable> Texture::Resolve(Graphics& gfx, const std::string& path, UINT slot, PipelineStage pipelineStage /*= PipelineStage::PixelShader*/)
{
	return BindableCodex::Resolve<Texture>(gfx, path, slot, pipelineStage);
}

std::string Texture::GenerateUID(const std::string& path, UINT slot, PipelineStage pipelineStage /*= PipelineStage::PixelShader*/)
{
	using namespace std::string_literals;
	return typeid(Texture).name() + "#"s + path + std::to_string(slot) + "#"s + std::to_string(static_cast<int>(pipelineStage));
}

std::string Texture::GetUID() const noexcept
{
	return GenerateUID(path, slot);
}
