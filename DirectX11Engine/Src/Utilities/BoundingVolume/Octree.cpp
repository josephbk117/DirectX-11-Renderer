#include "Octree.h"

void Octree::CalculateBounds(MinMaxVertexPair* out, Octant octant, MinMaxVertexPair parentRegion)
{
	DirectX::XMFLOAT3 center = parentRegion.GetMid();

	switch (octant)
	{
	case Octree::Octant::Q1:
		break;
	case Octree::Octant::Q2:
		break;
	case Octree::Octant::Q3:
		break;
	case Octree::Octant::Q4:
		break;
	case Octree::Octant::Q5:
		break;
	case Octree::Octant::Q6:
		break;
	case Octree::Octant::Q7:
		break;
	case Octree::Octant::Q8:
		break;
	default:
		break;
	}
}

Octree::Node::Node()
{

}

Octree::Node::Node(MinMaxVertexPair bounds)
{
	boundingBox = bounds;
}

Octree::Node::Node(MinMaxVertexPair bounds, std::vector<MinMaxVertexPair> objectList)
{
	boundingBox = bounds;
	objects.insert(objects.end(), objectList.begin(), objectList.end());
}

void Octree::Node::Build()
{

}

void Octree::Node::Update()
{

}

void Octree::Node::ProcessPending()
{

}

bool Octree::Node::Insert(MinMaxVertexPair obj)
{
	return false;
}

void Octree::Node::Destroy()
{

}
