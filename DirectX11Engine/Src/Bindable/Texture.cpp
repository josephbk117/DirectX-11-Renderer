#include "Texture.h"
#include "../Image.h"
#include "BindableCodex.h"

Texture::Texture(Graphics& gfx, Image& img, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos, bool genMipMap /*= true*/) : pipelineStageInfos(pipelineStageInfos)
{
	D3D11_TEXTURE2D_DESC td = {};
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.Width = img.GetWidth();
	td.Height = img.GetHeight();
	td.MipLevels = (genMipMap) ? 0 : 1; // Max number of mip levels of mipmap mode selected
	td.ArraySize = 1;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = (genMipMap) ? D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET : D3D11_BIND_SHADER_RESOURCE; // D3D11_BIND_RENDER_TARGET to allow for creating mips
	td.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	td.MiscFlags = (genMipMap) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0; // If no need for mips can be set null

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;

	if (!genMipMap)
	{
		D3D11_SUBRESOURCE_DATA sd;
		sd.pSysMem = img.GetData();
		sd.SysMemPitch = img.GetWidth() * sizeof(Image::Color);
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
	srv.Texture2D.MipLevels = (genMipMap) ? -1 : 1; // Can set to 1 if not using mip mapping

	GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srv, &pTextureView);

	//generate mip chain on GPU
	if (genMipMap)
	{
		GetContext(gfx)->GenerateMips(pTextureView.Get());
	}
}

Texture::Texture(Graphics& gfx, ImageHDR& img, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos, bool genMipMap /*= true*/) : pipelineStageInfos(pipelineStageInfos)
{
	D3D11_TEXTURE2D_DESC td = {};
	td.Format = img.format;
	td.Width = img.GetWidth();
	td.Height = img.GetHeight();
	td.MipLevels = (genMipMap) ? 0 : 1; // Max number of mip levels of mipmap mode selected
	td.ArraySize = 1;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = (genMipMap) ? D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET : D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS; // D3D11_BIND_RENDER_TARGET to allow for creating mips
	td.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	td.MiscFlags = (genMipMap) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0; // If no need for mips can be set null

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;

	if (!genMipMap)
	{
		D3D11_SUBRESOURCE_DATA sd{};
		sd.pSysMem = img.GetData();
		sd.SysMemPitch = img.GetWidth() * sizeof(ImageHDR::ColorFloat);
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
	srv.Texture2D.MipLevels = (genMipMap) ? -1 : 1; // Can set to 1 if not using mip mapping

	GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srv, &pTextureView);

	//generate mip chain on GPU
	if (genMipMap)
	{
		GetContext(gfx)->GenerateMips(pTextureView.Get());
	}
	else
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC descUAV;
		ZeroMemory(&descUAV, sizeof(descUAV));
		descUAV.Format = DXGI_FORMAT_UNKNOWN;
		descUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		descUAV.Texture2D.MipSlice = 0;
		GetDevice(gfx)->CreateUnorderedAccessView(pTexture.Get(), &descUAV, &pUAV);
	}
}

Texture::Texture(Graphics& gfx, const std::string& path, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos, bool genMipMap /*= true*/) : path(path), pipelineStageInfos(pipelineStageInfos)
{
	const auto img = Image::FromFile(path);

	D3D11_TEXTURE2D_DESC td = {};
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.Width = img.GetWidth();
	td.Height = img.GetHeight();
	td.MipLevels = (genMipMap) ? 0 : 1; // Max number of mip levels of mipmap mode selected
	td.ArraySize = 1;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = (genMipMap) ? D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET : D3D11_BIND_SHADER_RESOURCE; // D3D11_BIND_RENDER_TARGET to allow for creating mips
	td.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	td.MiscFlags = (genMipMap) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0; // If no need for mips can be set null

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;

	if (!genMipMap)
	{
		D3D11_SUBRESOURCE_DATA sd;
		sd.pSysMem = img.GetData();
		sd.SysMemPitch = img.GetWidth() * sizeof(Image::Color);
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
	srv.Texture2D.MipLevels = (genMipMap) ? -1 : 1; // Can set to 1 if not using mip mapping

	GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srv, &pTextureView);


	//generate mip chain on GPU
	if (genMipMap)
	{
		GetContext(gfx)->GenerateMips(pTextureView.Get());
	}
}


void Texture::Bind(Graphics& gfx) noexcept
{
	for (const PipelineStageSlotInfo& psi : pipelineStageInfos)
	{
		switch (psi.stage)
		{
		case PipelineStage::VertexShader:
			GetContext(gfx)->VSSetShaderResources(psi.slot, 1, pTextureView.GetAddressOf());
			break;
		case PipelineStage::HullShader:
			GetContext(gfx)->HSSetShaderResources(psi.slot, 1, pTextureView.GetAddressOf());
			break;
		case PipelineStage::DomainShader:
			GetContext(gfx)->DSSetShaderResources(psi.slot, 1, pTextureView.GetAddressOf());
			break;
		case PipelineStage::PixelShader:
			GetContext(gfx)->PSSetShaderResources(psi.slot, 1, pTextureView.GetAddressOf());
			break;
		case PipelineStage::ComputeShader:
			GetContext(gfx)->CSSetUnorderedAccessViews(psi.slot, 1, pUAV.GetAddressOf(), 0);
			break;
		default:
			GetContext(gfx)->PSSetShaderResources(psi.slot, 1, pTextureView.GetAddressOf());
			break;
		}
	}
}

void Texture::CustomBind(Graphics& gfx, PipelineStageSlotInfo customStageInfo) noexcept
{
	switch (customStageInfo.stage)
	{
	case PipelineStage::VertexShader:
		GetContext(gfx)->VSSetShaderResources(customStageInfo.slot, 1, pTextureView.GetAddressOf());
		break;
	case PipelineStage::HullShader:
		GetContext(gfx)->HSSetShaderResources(customStageInfo.slot, 1, pTextureView.GetAddressOf());
		break;
	case PipelineStage::DomainShader:
		GetContext(gfx)->DSSetShaderResources(customStageInfo.slot, 1, pTextureView.GetAddressOf());
		break;
	case PipelineStage::PixelShader:
		GetContext(gfx)->PSSetShaderResources(customStageInfo.slot, 1, pTextureView.GetAddressOf());
		break;
	case PipelineStage::ComputeShader:
		GetContext(gfx)->CSSetUnorderedAccessViews(customStageInfo.slot, 1, pUAV.GetAddressOf(), 0);
		break;
	default:
		GetContext(gfx)->PSSetShaderResources(customStageInfo.slot, 1, pTextureView.GetAddressOf());
		break;
	}
}

void Texture::UnBind(Graphics& gfx) noexcept
{
	for (PipelineStageSlotInfo psi : pipelineStageInfos)
	{
		CustomUnBind(gfx, psi);
	}
}

void Texture::CustomUnBind(Graphics& gfx, PipelineStageSlotInfo customStageInfo) noexcept
{
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	switch (customStageInfo.stage)
	{
	case PipelineStage::VertexShader:
		GetContext(gfx)->VSSetShaderResources(customStageInfo.slot, 1, nullSRV);
		break;
	case PipelineStage::HullShader:
		GetContext(gfx)->HSSetShaderResources(customStageInfo.slot, 1, nullSRV);
		break;
	case PipelineStage::DomainShader:
		GetContext(gfx)->DSSetShaderResources(customStageInfo.slot, 1, nullSRV);
		break;
	case PipelineStage::PixelShader:
		GetContext(gfx)->PSSetShaderResources(customStageInfo.slot, 1, nullSRV);
		break;
	case PipelineStage::ComputeShader:
	{
		ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
		GetContext(gfx)->CSSetUnorderedAccessViews(customStageInfo.slot, 1, nullUAV, 0);
	}
	break;
	default:
		GetContext(gfx)->PSSetShaderResources(customStageInfo.slot, 1, nullSRV);
		break;
	}
}

std::shared_ptr<Bindable> Texture::Resolve(Graphics& gfx, const std::string& path, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos)
{
	return BindableCodex::Resolve<Texture>(gfx, path, pipelineStageInfos);
}

std::string Texture::GenerateUID(const std::string& path, const std::vector<PipelineStageSlotInfo>& pipelineStageInfos)
{
	using namespace std::string_literals;
	std::string completeStr = path;
	for (const PipelineStageSlotInfo& psi : pipelineStageInfos)
	{
		completeStr += std::to_string(psi.slot) + "#"s + std::to_string(static_cast<int>(psi.stage));
	}
	return typeid(Texture).name() + "#"s + completeStr;
}

std::string Texture::GetUID() const noexcept
{
	return GenerateUID(path, pipelineStageInfos);
}
