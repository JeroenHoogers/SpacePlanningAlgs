#pragma once

#include "ofMain.h"

// Genotype structure
template<typename T>
struct Genotype
{
	vector<T> genes;
	float fitness;

	Genotype() : genes(vector<T>()), fitness(0.0f) { };
	Genotype(vector<T> _genes) : genes(_genes), fitness(0.0f) { };
	Genotype(vector<T> _genes, float _fitness) : genes(_genes), fitness(_fitness) { };
};

enum class EMatingStrategy { 
	SwitchSource,	// switch source parent at some frequency
	Gene,			// 
	Interpolate		// generate a percentage p per gene and interpolate the gene value based on this percentage v = p1(p) + p2(1-p)
};

// TODO: implement as abstract template class instead to allow multiple implementations


template <typename T> // class type  
class GeneticAlgorithm
{
private:
								// TODO: could use gaussian distribution for mutation to decrease the likelyness of radical changes
	bool elitism = true;		// Let the best few of a population survive without mutation
	float elitismRate = 0.05f;

public:
	vector<Genotype<T>> population;
	vector<int> selectedIndices;

	EMatingStrategy matingStrat;

	int populationSize = 20;

	int numGenes; // parameters

	float mutationRate = 0.25f; // probability of mutating a single gene
	float mutationAmount = 0.4f; // maximum mutation amount compared to original gene value (offset) 
	
	float crossoverRate = 0.75f;

	bool groupGenes = false;
	int groupSize = 5;

	int currentGeneration = 0;
	
	// TODO: always add the selection of the previous population to the new offspring otherwise, the solution might never converge
	GeneticAlgorithm();
	~GeneticAlgorithm();

	void setup(int popSize, int n);
	void setup(int popSize, int n, float mutRate, float mutAmount);

	//float absDifference(int index, DNA target);
	void generateRandomPopulation();
	vector<T> generateRandomDna();

	virtual void mutate(vector<T>* genotype);

	void generateOffspring();
	//void generateOffspring(const vector<float>& fitnesses);

	// select a single individual from the population for offspring
	void select(int index);
	void selectByFitness(const vector<float>& fitnesses);

	vector<T> rouletteSelection(float totalFitness);

	virtual void crossover(vector<T>* offspring1, vector<T>* offspring2);

	//DNA crossover(const DNA& parent1, const DNA& parent2, float probability);
	//DNA crossoverInterpolation(const DNA& parent1, const DNA& parent2);
	//DNA crossoverSwitchGenotype(const DNA& parent1, const DNA& parent2, int frequency);
};


//template <> // genotype: float
//class GeneticAlgorithm<float>
//{
//	virtual void mutate(vector<float>* genotype);
//	vector<float> generateRandomDna();
//};

// specialized populate methods
template <>
vector<float> GeneticAlgorithm<float>::generateRandomDna();

template <>
vector<bool> GeneticAlgorithm<bool>::generateRandomDna();


// specialized mutation methods
template <>
virtual void GeneticAlgorithm<float>::mutate(vector<float>* genotype);

template <>
virtual void GeneticAlgorithm<bool>::mutate(vector<bool>* genotype);
