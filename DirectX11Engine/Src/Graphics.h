#pragma once
#include "GlobalDefines.h"
#include "EngineException.h"
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>

class Graphics
{
	friend class Bindable;
public:

	struct AdapterInfo
	{
		std::string desc;
		UINT vendorId;
		SIZE_T dedicatedVideoMemory;
		SIZE_T dedicatedSystemMemory;
		SIZE_T sharedSystemMemory;
	};

	class HrException : public EngineException
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
public:
	Graphics(HWND hWnd, int width, int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;

	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept;
	void Draw(UINT vertexCount)noexcept;
	void DrawIndexed(UINT indexCount)noexcept;
	void DrawIndexedInstanced(UINT indexCount, UINT instanceCount)noexcept;
	void DrawDebugLines(const std::vector<DirectX::XMFLOAT3>& linePoints, DirectX::XMMATRIX objectTransform)noexcept;
	void Dispatch(UINT x, UINT y, UINT z) noexcept;
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(DirectX::FXMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;
	void SetCameraPos(DirectX::XMVECTOR camPos) noexcept;
	DirectX::XMVECTOR GetCameraPos() const noexcept;

	UINT GetAdapterCount() const noexcept;
	AdapterInfo GetAdapterInfo(int index) const noexcept;
	IDXGIAdapter* GetSuitableDevice() const noexcept;

private:
	void EnumerateAdapters() const;

	DirectX::XMMATRIX projection = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
	DirectX::XMVECTOR camPos;

	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSawp = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV = nullptr;

	mutable std::vector<AdapterInfo> adapterInfos;
};

