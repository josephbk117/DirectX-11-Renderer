#include "Camera.h"
#include <algorithm>

Camera::Camera() noexcept
{
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	using namespace DirectX;

	const XMVECTOR forwardVector = { 0.0f, 0.0f, 1.0f, 0.0f };

	const auto lookVector = XMVector3Transform(forwardVector, XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f));

	const auto camPos = DirectX::XMLoadFloat3(&pos);
	const auto camTarget = camPos + lookVector;

	return XMMatrixLookAtLH(camPos, camTarget, { 0.0f, 1.0f,0.0f, 0.0f });
}

DirectX::XMVECTOR Camera::GetPosition() const noexcept
{
	return DirectX::XMLoadFloat3(&pos);
}

void Camera::Reset() noexcept
{
	pos = { 0.0f, 0.0, 0.0f };
	pitch = 0.0f;
	yaw = 0.0f;
}

void Camera::Rotate(float dx, float dy) noexcept
{

	yaw += dx * ROTATION_SPEED;

	constexpr float PI = 22.0f / 7.0f;
	constexpr double PI_D = 3.1415926535897932;
	constexpr float twoPi = (float)2 * (float)PI_D;
	const float mod = (float)fmod(yaw, twoPi);
	if (mod > (float)PI_D)
	{
		yaw =  mod - twoPi;
	}
	else if (mod < -(float)PI_D)
	{
		yaw =  mod + twoPi;
	}

	pitch = std::clamp(pitch + dy * ROTATION_SPEED, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	DirectX::XMStoreFloat3(&translation, 
		DirectX::XMVector3Transform(
			DirectX::XMLoadFloat3(&translation),
			DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
			DirectX::XMMatrixScaling(TRAVERSAL_SPEED, TRAVERSAL_SPEED, TRAVERSAL_SPEED)
	));

	pos =
	{
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z
	};
}
