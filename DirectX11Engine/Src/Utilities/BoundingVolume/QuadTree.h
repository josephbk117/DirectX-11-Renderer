#pragma once
#include "BoundingBox.h"
#include <vector>
#include "../../Graphics.h"

class QuadTree
{
public:
	QuadTree() = default;
	QuadTree(MinMaxVertexPair boundary);

	bool Insert(const DirectX::XMFLOAT3& point);
	void Subdivide();

	void DrawDebug(Graphics& gfx);

private:
	MinMaxVertexPair boundary;
	size_t capacity = 1;
	std::vector<DirectX::XMFLOAT3> objs;

	bool isDivided = false;

	QuadTree* TL = nullptr;
	QuadTree* TR = nullptr;
	QuadTree* BL = nullptr;
	QuadTree* BR = nullptr;
};

 