#pragma once
#include "GeneticAlgorithm.h"
#include "GeneticTreeAlgorithm.h"
#include "Evolver.h"
#include "IntersectionHelper.h"

struct SplitTreeNode 
{
	int index;				// stores either the room index or the split index, depending on whether this is a leaf or not

	SplitTreeNode* leftChild;   // Pointer to the left subtree.
	SplitTreeNode* rightChild;  // Pointer to the right subtree.

	SplitTreeNode(int _index)
	{
		index = _index;
		leftChild = NULL;
		rightChild = NULL;
	}

	SplitTreeNode(int _index, SplitTreeNode* _leftChild, SplitTreeNode* _rightChild)
	{
		index = _index;
		leftChild = _leftChild;
		rightChild = _rightChild;
	}

	~SplitTreeNode()
	{
		delete leftChild;
		delete rightChild;
	}

	bool isLeaf()
	{
		return (leftChild == NULL || rightChild == NULL);
	}

};

struct Split
{
	float position;

	// 0: x-axis, 1: y-axis
	int axis;

	Split(float _position, int _axis)
	{
		position = _position;
		axis = _axis;
	}
};

// Room data structure
struct InteriorRoom
{
	Room* pRoom;
	ofPolyline shape;

	InteriorRoom(Room* _pRoom, ofPolyline _shape)
	{
		pRoom = _pRoom;
		shape = _shape;
	}

	float getArea()
	{
		return abs(shape.getArea());
	}
};


// This is the layer between the genetic algorithm and the architecture state
class InteriorEvolver : public Evolver
{
private:

	// amount of splits in the binary tree
	int splits;

	// amount of rooms (leaves in the binary tree)
	int rooms;

	// number of generations used by the room sizing algorithm
	int optimizationGenerations = 20;

	GeneticAlgorithm roomOptimizationAlgorithm;
	GeneticTreeAlgorithm geneticTreeAlgorithm;

	// use this for tree structure evolution
	vector<SplitTreeNode*> trees;

	vector<vector<InteriorRoom>> interiors;
	// TODO: 

public:
	// TODO: allow multiple floors
	ofPolyline floorshape;

	InteriorEvolver();
	~InteriorEvolver();

	void setup(int _tiles, ArchitectureProgram* _pProgram);
	void setFloorShape(ofPolyline _floorshape);

	vector<InteriorRoom>* getInteriorAt(int tile);

	void generate(vector<int> selection);

	vector<InteriorRoom> optimizeInterior(int treeIndex);

	float computeInteriorFitness(const vector<Split>& splits, int treeIndex);

	vector<InteriorRoom> generateRooms(const vector<Split>& splits, SplitTreeNode* node, const ofPolyline& shape);

	SplitTreeNode* constructTestTree();
};

