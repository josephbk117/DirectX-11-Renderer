#pragma once
#include <DirectXMath.h>
struct MinMaxVertexPair
{
	MinMaxVertexPair() = default;
	MinMaxVertexPair(DirectX::XMFLOAT3 minVal, DirectX::XMFLOAT3 maxVal) : min(minVal), max(maxVal)
	{
		mid.x = (minVal.x + maxVal.x) * 0.5f;
		mid.y = (minVal.y + maxVal.y) * 0.5f;
		mid.z = (minVal.z + maxVal.z) * 0.5f;
	}

	void SetMinMax(DirectX::XMFLOAT3 minVal, DirectX::XMFLOAT3 maxVal)
	{
		min = minVal;
		max = maxVal;

		mid.x = (minVal.x + maxVal.x) * 0.5f;
		mid.y = (minVal.y + maxVal.y) * 0.5f;
		mid.z = (minVal.z + maxVal.z) * 0.5f;
	}

	DirectX::XMFLOAT3 GetMin() const
	{
		return min;
	}

	DirectX::XMFLOAT3 GetMax() const
	{
		return max;
	}

	DirectX::XMFLOAT3 GetMid() const
	{
		return mid;
	}

	float GetSurfaceArea() const
	{
		return 1.0f;
	}

private:
	DirectX::XMFLOAT3 min = { 0,0,0 };
	DirectX::XMFLOAT3 max = { 0,0,0 };
	DirectX::XMFLOAT3 mid = { 0,0,0 };
};
