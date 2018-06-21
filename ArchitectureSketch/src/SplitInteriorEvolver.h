#pragma once
#include "GeneticAlgorithm.h"
//#include "GeneticTreeAlgorithm.h"
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

	bool isLeaf() const
	{
		return (leftChild == NULL || rightChild == NULL);
	}

	void print() 
	{
		print("", this, false);
	}

	void print(string prefix, SplitTreeNode* node, bool isLeft) 
	{
		if (node != NULL) {
			cout << prefix << (isLeft ? "|-- " : "\\-- ") << node->index << endl;
			print(prefix + (isLeft ? "|   " : "    "), node->leftChild, true);
			print(prefix + (isLeft ? "|   " : "    "), node->rightChild, false);
		}
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
	ofRectangle bb;

	InteriorRoom(Room* _pRoom, ofPolyline _shape)
	{
		pRoom = _pRoom;
		shape = _shape;
		bb = shape.getBoundingBox();
	}

	float getArea() const
	{
		return abs(shape.getArea());
	}

	float getMinDim() const
	{
		return fminf(bb.getWidth(), bb.getHeight());
	}

	float getMaxDim() const
	{
		return fmaxf(bb.getWidth(), bb.getHeight());
	}
};


// This is the layer between the genetic algorithm and the architecture state
class SplitInteriorEvolver : public Evolver
{
private:

	// amount of splits in the binary tree
	int splits;

	// amount of rooms (leaves in the binary tree)
	int nRooms;

	// number of generations used by the room sizing algorithm
	int optimizationGenerations = 70;
	//int gen = 0;

	GeneticAlgorithm<float> roomOptimizationAlgorithm;
	// TODO: add tree generation
	GeneticAlgorithm<float> geneticTreeAlgorithm;
	GeneticAlgorithm<float> adjacencyWeightsAlgorithm;

	// use this for tree structure evolution
	vector<SplitTreeNode*> trees;

	vector<vector<InteriorRoom>> interiors;

public:
	// TODO: allow multiple floors
	ofPolyline floorshape;

	SplitInteriorEvolver();
	~SplitInteriorEvolver();

	void setup(int _tiles, ArchitectureProgram* _pProgram);
	void setFloorShape(ofPolyline _floorshape);

	vector<InteriorRoom>* getInteriorAt(int tile);

	void generate(vector<int> selection);

	vector<InteriorRoom> optimizeInterior(int treeIndex);

	void optimizeInteriors();

	float computeInteriorFitness(const vector<Split>& splits, int treeIndex);
	bool checkAdjacency(const InteriorRoom& r1, const InteriorRoom& r2);

	void generateRooms(const vector<Split>& splits, const SplitTreeNode* node, const ofPolyline& shape, vector<InteriorRoom>& rooms);

	SplitTreeNode* constructTestTree();
	SplitTreeNode* constructTestTree2();

	SplitTreeNode* constructTree(const vector<float>& treeGenome, int i, int n);

	void orderLeaves(SplitTreeNode* root);

	void drawDebug(ofPoint p, int tile) override;
};

