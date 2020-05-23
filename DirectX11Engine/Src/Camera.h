#pragma once
#include <DirectXMath.h>
class Camera
{
public:
	Camera() noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void Reset() noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
private:
	DirectX::XMFLOAT3 pos;
	float pitch;
	float yaw;

	static constexpr float TRAVERSAL_SPEED = 15.0f;
	static constexpr float ROTATION_SPEED = 0.002f;
};

