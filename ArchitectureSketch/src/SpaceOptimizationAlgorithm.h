#pragma once
#include "GeneticAlgorithm.h"

class SpaceOptimizationAlgorithm :
	public GeneticAlgorithm
{
private:
	void calculateFitness();
	void selectIndividuals();

public:
	SpaceOptimizationAlgorithm();
	~SpaceOptimizationAlgorithm();


};

