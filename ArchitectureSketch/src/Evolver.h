#pragma once
#include "ofMain.h"
#include "ArchitectureProgram.h"

class Evolver
{
protected:
	int tiles;
	ArchitectureProgram* program;
	vector<int> selected;

public:
	Evolver();
	~Evolver();

	void select(int index);
	virtual void generate() = 0;
};

