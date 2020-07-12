#include "Graphics.h"
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Bindable/VertexBuffer.h"
#include "Imgui/imgui_impl_dx11.h"
#include "Utilities/EngineUtilities.h"
#include <dxgi.h>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")

#define GFX_THROW_FAILED(hrCall) if(FAILED(hr = (hrCall))) throw Graphics::HrException(__LINE__, __FILE__, (hr))

Graphics::Graphics(HWND hWnd, int width, int height)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	HRESULT hr;

	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION,
		&sd, &pSawp, &pDevice, nullptr, &pContext));

	EnumerateAdapters();

	wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
	pSawp->GetBuffer(0, _uuidof(ID3D11Resource), &pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget);

	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;

	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &pDSV);

	pContext->OMSetRenderTargets(1, pTarget.GetAddressOf(), pDSV.Get());

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);

	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());

}

void Graphics::EndFrame()
{
	HRESULT hr;

	if (FAILED(hr = pSawp->Present(1, 0)))
	{
		;
	}
	else
	{
		GFX_THROW_FAILED(hr);
	}
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float color[] = { r,g,b,1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Graphics::DrawIndexed(UINT indexCount) noexcept
{
	pContext->DrawIndexed(indexCount, 0, 0);
}

void Graphics::DrawIndexedInstanced(UINT indexCount, UINT instanceCount) noexcept
{
	pContext->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}

void Graphics::SetCamera(DirectX::FXMMATRIX cam) noexcept
{
	view = cam;
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return view;
}

void Graphics::SetCameraPos(DirectX::XMVECTOR camPos) noexcept
{
	this->camPos = camPos;
}

DirectX::XMVECTOR Graphics::GetCameraPos() const noexcept
{
	return camPos;
}

UINT Graphics::GetAdapterCount() const noexcept
{
	return adapterInfos.size();
}

Graphics::AdapterInfo Graphics::GetAdapterInfo(int index) const noexcept
{
	return adapterInfos.at(index);
}

void Graphics::EnumerateAdapters()
{
	IDXGIAdapter* pAdapter;
	IDXGIFactory* pFactory = nullptr;

	// Create a DXGIFactory object.
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)& pFactory)))
	{
		return;
	}

	for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC desc = {};
		pAdapter->GetDesc(&desc);
		AdapterInfo info = { Utility::WStringToString(std::wstring(desc.Description)), desc.VendorId, desc.DedicatedVideoMemory, desc.DedicatedSystemMemory, desc.SharedSystemMemory };
		adapterInfos.push_back(info);

		pAdapter->Release();
	}

	if (pFactory)
	{
		pFactory->Release();
	}
}

Graphics::HrException::HrException(int line, const char* file, HRESULT hr) noexcept :
	EngineException(line, file), hr(hr)
{}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl << "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetErrorDescription();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Engine Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr);

	if (nMsgLen == 0)
	{
		return "Undefined Error Code";
	}
	std::string errorStr = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorStr;
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	return "";
}
