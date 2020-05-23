#pragma once
#include <queue>
#include <bitset>
#include <optional>

class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press, Release, Invalid
		};
		
	private:
		Type type;
		unsigned char code;
	public:
		Event() : type(Type::Invalid), code(0u) {}
		Event(Type type, unsigned char code) noexcept : type(type), code(code) {}
		bool IsPressed() const noexcept { return type == Type::Press; }
		bool IsReleased() const noexcept { return type == Type::Release; }
		unsigned char GetCode() const noexcept { return code; }
	};

	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	bool KeyIsPressed(unsigned char keycode) const noexcept;
	std::optional<Keyboard::Event> ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;

	std::optional<char> ReadChar() noexcept;
	bool IsCharEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;

	void EnableAutoRepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool IsAutoRepeatEnabled() const noexcept;
private:
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;

	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keyStates;
	std::queue<Event> keyBuffer;
	std::queue<char> charBuffer;
};
