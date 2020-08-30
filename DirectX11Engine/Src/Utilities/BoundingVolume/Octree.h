#pragma once
#include <vector>
#include <queue>
#include <stack>
#include "BoundingBox.h"

class Octree
{
public:
	enum class Octants : unsigned char
	{
		Q1 = 0x01,
		Q2 = 0x02,
		Q3 = 0x03,
		Q4 = 0x04,
		Q5 = 0x05,
		Q6 = 0x06,
		Q7 = 0x07,
		Q8 = 0x08
	};

	struct Node
	{
		Node* parent = nullptr;
		Node* children[4];

		unsigned char activeOctants;

		bool hasChildren = false;
		bool treeReady = false;
		bool treeBuilt = false;
		
		std::vector<MinMaxVertexPair> objects;
		std::queue<MinMaxVertexPair> queue;

		MinMaxVertexPair boundingBox;
	};

};

