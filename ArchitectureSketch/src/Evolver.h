#pragma once
#include "ofMain.h"
#include "ArchitectureProgram.h"
#include "GeneticAlgorithm.h"

class Evolver
{
protected:
	int tiles;
	ArchitectureProgram* pProgram;

	// list of candidates after filtering
	vector<int> candidates;

public:

	int generation = 0;

	Evolver();
	~Evolver();

	void setup(int _tiles, ArchitectureProgram* _pProgram);

	void select(int index);
	virtual void generate(vector<int> selection) = 0;
};

