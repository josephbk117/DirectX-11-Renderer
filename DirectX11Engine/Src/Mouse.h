#pragma once
#include "GlobalDefines.h"
#include <utility>
#include <queue>
#include <optional>
class Mouse
{
	friend class Window;

public:
	struct RawDelta
	{
		int x = 0, y = 0;
	};

	class  Event
	{
	public:
		enum class Type
		{
			Lpress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};
	private:
		Type type;
		bool leftIsPressed;
		bool rightIsPressed;
		int x, y;
	public:
		Event() noexcept : type(Type::Invalid), leftIsPressed(false), rightIsPressed(false), x(0), y(0) {}
		Event(Type type, const Mouse& parent) noexcept : type(type),
			leftIsPressed(parent.leftIsPressed), rightIsPressed(parent.rightIsPressed),
			x(parent.x), y(parent.y) {}
		bool IsValid() const noexcept { return type != Type::Invalid; }
		Type GetType() const noexcept { return type; }
		std::pair<int, int> GetPos() const noexcept { return { x, y }; }
		int GetPosX() const noexcept { return x; }
		int GetPosY() const noexcept { return y; }
		bool IsLeftPressed()const noexcept { return leftIsPressed; }
		bool IsRightPressed() const noexcept { return rightIsPressed; }
	};
public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	std::pair<int, int> GetPos() const noexcept;
	std::optional<RawDelta> ReadRawDelta() noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	int GetScroll() const noexcept;
	bool IsInWindow() const noexcept;
	bool IsLeftPressed() const noexcept;
	bool IsRightPressed() const noexcept;
	Mouse::Event Read() noexcept;
	bool IsEmpty() const noexcept;
	void Flush() const noexcept;
private:
	void OnMouseMove(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void OnWheelDelta(int x, int y, int delta) noexcept;
	void OnRawDelta(int x, int y)noexcept;
	void TrimBuffer() noexcept;
	void TrimRawInputBuffer() noexcept;

	static constexpr unsigned int bufferSize = 16u;
	int x = 0, y = 0;
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	bool mouseIsInWindow = true;
	int wheelDeltaCarry = 0;
	mutable std::queue<Event> buffer;
	mutable std::queue<RawDelta> rawDeltaBuffer;
};
