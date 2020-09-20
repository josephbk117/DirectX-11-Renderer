#include "QuadTree.h"
#include "../EngineUtilities.h"

QuadTree::QuadTree(MinMaxVertexPair boundary) : boundary(boundary)
{

}

bool QuadTree::Insert(const DirectX::XMFLOAT3& point)
{
	if (!boundary.ContainsPoint(point))
	{
		return false;
	}

	if (objs.size() < capacity)
	{
		objs.push_back(point);
	}
	else
	{
		if (!isDivided)
		{
			Subdivide();
			isDivided = true;
		}

		if (TR->Insert(point)) {}
		else if (TL->Insert(point)) {}
		else if (BR->Insert(point)) {}
		else if (BL->Insert(point)) {}
	}
	return true;
}

void QuadTree::Subdivide()
{
	using namespace DirectX;

	XMFLOAT3 topRight = boundary.GetMax();
	XMFLOAT3 topLeft = { boundary.GetMin().x, boundary.GetMax().y, 0 };
	XMFLOAT3 bottomRight = { boundary.GetMax().x, boundary.GetMin().y, 0 };
	XMFLOAT3 bottomLeft = boundary.GetMin();

	XMFLOAT3 mid = boundary.GetMid();
	XMFLOAT3 midLeft = { bottomLeft.x, mid.y, 0 };
	XMFLOAT3 midRight = { topRight.x, mid.y, 0 };
	XMFLOAT3 midTop = { mid.x, topRight.y, 0 };
	XMFLOAT3 midbtm = { mid.x, bottomLeft.y, 0 };

	MinMaxVertexPair trVP;
	trVP.SetMinMax(mid, topRight);
	TR = new QuadTree(trVP);

	MinMaxVertexPair tlVP;
	tlVP.SetMinMax(midLeft, midTop);
	TL = new QuadTree(tlVP);

	MinMaxVertexPair brVP;
	brVP.SetMinMax(midbtm, midRight);
	BR = new QuadTree(brVP);

	MinMaxVertexPair blVP;
	blVP.SetMinMax(bottomLeft, mid);
	BL = new QuadTree(blVP);

}

void QuadTree::DrawDebug(Graphics& gfx)
{
	const DirectX::XMFLOAT3 topLeft = { boundary.GetMin().x, boundary.GetMax().y, 0 };
	const DirectX::XMFLOAT3 bottomRight = { boundary.GetMax().x, boundary.GetMin().y, 0 };
	const std::vector<DirectX::XMFLOAT3> linePairs =
	{
		topLeft, boundary.GetMax(),
		boundary.GetMax(), bottomRight,
		bottomRight, boundary.GetMin(),
		boundary.GetMin(), topLeft
	};

	gfx.DrawDebugLines(linePairs, DirectX::XMMatrixIdentity());
	gfx.SetDebugDrawColour({ 1,1,0,1 });
	for (const DirectX::XMFLOAT3& vp : objs)
	{
		gfx.DrawDebugCircle(vp, 1.0f, DirectX::XMMatrixIdentity());
	}
	gfx.SetDebugDrawColour({ 1,0,0,1 });
	if (TR)
	{
		TR->DrawDebug(gfx);
	}
	if (TL)
	{
		TL->DrawDebug(gfx);
	}
	if (BR)
	{
		BR->DrawDebug(gfx);
	}
	if (BL)
	{
		BL->DrawDebug(gfx);
	}
}
