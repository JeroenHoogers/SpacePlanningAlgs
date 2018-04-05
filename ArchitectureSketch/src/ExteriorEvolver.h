#pragma once
#include "Evolver.h"
#include "Building.h"

class ExteriorEvolver : public Evolver
{
private:
	GeneticAlgorithm geneticAlgorithm;
	vector<Building> buildings;

public:

	ExteriorEvolver();
	~ExteriorEvolver();

	void setup(int _tiles, ArchitectureProgram* _pProgram);

	void generate(vector<int> selection);

	bool isBuildingValid(Building& building);

	Building* getBuildingAt(int tile);
};

