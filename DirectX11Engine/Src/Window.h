#pragma once
#include "GlobalDefines.h"
#include "EngineException.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Camera.h"
#include <memory>
#include <optional>
class Window
{
public:
	class Exception : public EngineException
	{
	public:
		Exception(int line, const char* file, HRESULT hr);
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr);
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};
private:
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass()noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "Dx11 Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	static void GetNativeScreenResolution(unsigned int& width, unsigned int& height) noexcept;

	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string& title);
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;
	void HideCursor() noexcept;
	void ShowCursor() noexcept;
	bool GetIsCursorVisible() const noexcept;
	bool GetIsCursorConfined() const noexcept;
	bool GetIsCursorEnabled() const noexcept;
	static std::optional<int> ProcessMessages();
	Graphics& Gfx();
	Keyboard kbd;
	Mouse mouse;
	Camera cam;
private:
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;
	static HRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static HRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	bool isCursorVisible = true;
	bool isCursorConfined = false;
	unsigned int width = 0;
	unsigned int height = 0;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
	std::vector<char> rawBuffer;
};
// Error exception helper macro
#define ENG_WND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr)
#define ENG_WND_LASTEXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError())
