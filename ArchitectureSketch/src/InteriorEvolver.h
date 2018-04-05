#pragma once
#include "GeneticAlgorithm.h"
#include "Evolver.h"
#include "IntersectionHelper.h"

struct SplitTreeNode 
{
	int item;					// The data in this node.
	SplitTreeNode* leftChild;   // Pointer to the left subtree.
	SplitTreeNode* rightChild;  // Pointer to the right subtree.
};

// This is the layer between the genetic algorithm and the architecture state
class InteriorEvolver : public Evolver
{
private:
	// TODO: allow multiple floors
	ofPolyline floorshape;
	int splits;
	GeneticAlgorithm geneticAlgorithm;

public:
	InteriorEvolver();
	~InteriorEvolver();

	void setup(ArchitectureProgram* program, ofPolyline floorshape);

	void generate(vector<int> selection);

	void computeFitness();
};

