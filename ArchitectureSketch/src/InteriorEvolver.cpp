#include "InteriorEvolver.h"

//--------------------------------------------------------------
InteriorEvolver::InteriorEvolver()
{
}

//--------------------------------------------------------------
InteriorEvolver::~InteriorEvolver()
{
}

//--------------------------------------------------------------
void InteriorEvolver::setup(ArchitectureProgram* program, ofPolyline floorshape)
{
	// calculate the number of splits
	splits = program->rooms.size() - 1;

	// evolve a binary tree with #rooms leafs and #splits interior nodes
}

//--------------------------------------------------------------
void InteriorEvolver::computeFitness()
{

}

//--------------------------------------------------------------
void InteriorEvolver::generate(vector<int> selection)
{

}
