#include "GeneticAlgorithm.h"

//--------------------------------------------------------------
GeneticAlgorithm::GeneticAlgorithm()
{

}

//--------------------------------------------------------------
GeneticAlgorithm::~GeneticAlgorithm()
{

}

//--------------------------------------------------------------
void GeneticAlgorithm::setup(int popSize, int n)
{
	populationSize = popSize;
	numGenes = n;

	// fill first gen with random data
	generateRandomPopulation();
}


//--------------------------------------------------------------
void GeneticAlgorithm::setup(int popSize, int n, float mutRate, float mutAmount)
{
	populationSize = popSize;
	numGenes = n;

	mutationRate = mutRate;
	mutationAmount = mutAmount;

	// fill first gen with random data
	generateRandomPopulation();
}

//--------------------------------------------------------------
void GeneticAlgorithm::generateRandomPopulation()
{
	population.clear();

	for (int i = 0; i < populationSize; i++)
	{
		population.push_back(generateRandomDna());
	}
}

//--------------------------------------------------------------
Genotype GeneticAlgorithm::generateRandomDna()
{
	Genotype genotype = Genotype();
	for (int i = 0; i < numGenes; i++)
	{
		genotype.push_back(ofRandom(1));
	}

	return genotype;
}

//--------------------------------------------------------------
void GeneticAlgorithm::select(int index)
{
	if(index >= 0 && index < population.size())
		selectedIndices.push_back(index);
}

//--------------------------------------------------------------
void GeneticAlgorithm::generateOffspring()
{
	
	if (selectedIndices.size() == 0) // no selection 
	{
		// clear population
		population.clear();

		// create new random population
		generateRandomPopulation();
	}
	else if (selectedIndices.size() == 1) // mutate single 
	{
		// mutate single
		Genotype selected = population[selectedIndices[0]];

		// clear population
		population.clear();

		// add selected individual (optional)
		population.push_back(selected);

		// populate the rest with mutations of the selected genotype
		for (int i = population.size(); i < populationSize; i++)
		{
			population.push_back(mutate(selected));
		}
	}
	
	// remove selection
	selectedIndices.clear();

	// increase generation counter
	currentGeneration++;
}

//--------------------------------------------------------------
Genotype GeneticAlgorithm::mutate(Genotype genotype)
{
	//vector<float> genotype;
	for (size_t i = 0; i < genotype.size(); i++)
	{
		// mutate this gene with probability equal to the mutation rate
		float r = ofRandom(1); //static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		if (r < mutationRate)
		{
			// adjust 
			genotype[i] += ofRandom(-mutationAmount, mutationAmount);

			// clamp the mutation to a legal value
			genotype[i] = ofClamp(genotype[i], 0, 1);
		}
	}

	return genotype;
}

//--------------------------------------------------------------
void GeneticAlgorithm::mate()
{
	// generate 2^N possible values and randomly select a few to be in the new population, also add the original parents

}
