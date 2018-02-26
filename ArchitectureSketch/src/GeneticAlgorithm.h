#pragma once
#include "ofMain.h"

enum class MatingStrategy{ Crossover, Corners, Interpolation };
enum class MutationStrategy { Combinatorial, Elitism };

typedef vector<float> Genotype;

// TODO: implement as abstract template class instead to allow multiple implementations
class GeneticAlgorithm
{
private:
	MatingStrategy matingStrat;
	MutationStrategy mutationStrat;
	
	int populationSize = 20;
	
	int numGenes; // parameters

	float mutationRate = 0.3; // probability of mutating a single gene
	float mutationAmount = 0.4; // maximum mutation amount compared to original gene value (offset) 
								// TODO: could use gaussian distribution for mutation to decrease the likelyness of radical changes
public:
	vector<Genotype> population;
	vector<int> selectedIndices;

	int currentGeneration = 0;
	
	// TODO: always add the selection of the previous population to the new offspring otherwise, the solution might never converge
	GeneticAlgorithm();
	~GeneticAlgorithm();

	void setup(int popSize, int n);
	void setup(int popSize, int n, float mutRate, float mutAmount);

	void generateRandomPopulation();
	Genotype generateRandomDna();

	Genotype mutate(Genotype genotype);

	void generateOffspring();

	// select a single individual from the population for offspring
	void select(int index);

	void mate();
};