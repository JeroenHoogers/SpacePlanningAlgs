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
	currentGeneration = 0; 

	populationSize = popSize;
	numGenes = n;

	// fill first gen with random data
	generateRandomPopulation();
}


//--------------------------------------------------------------
void GeneticAlgorithm::setup(int popSize, int n, float mutRate, float mutAmount)
{
	currentGeneration = 0;

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
		if (allowSurvival)
		{
			population.push_back(selected);
		}

		// populate the rest with mutations of the selected genotype
		for (int i = population.size(); i < populationSize; i++)
		{
			population.push_back(mutate(selected));
		}
	}
	else if (selectedIndices.size() == 2) // mate
	{
		// mate 2 parents
		Genotype parent1 = population[selectedIndices[0]];
		Genotype parent2 = population[selectedIndices[1]];

		// clear population
		population.clear();

		// add selected individuals (optional)
		if (allowSurvival)
		{
			population.push_back(parent1);
			population.push_back(parent2);
		}

		// populate the rest with mutations of the selected genotype
		for (int i = population.size(); i < populationSize; i++)
		{
			Genotype child = mate(parent1, parent2, 0.5f);
			//population.push_back(child);
			population.push_back(mutate(child));
		}
	}
	else if (selectedIndices.size() > 2)
	{
		vector<Genotype> parents;

		for (int i = 0; i < selectedIndices.size(); i++)
		{
			parents.push_back(population[selectedIndices[i]]);
		}

		// clear population
		population.clear();

		// add selected individuals (optional)
		if (allowSurvival)
		{
			for (int i = 0; i < parents.size(); i++)
			{
				population.push_back(parents[i]);
			}
		}

		float prob = 1.0f / selectedIndices.size();

		// populate the rest with mutations of the selected genotype
		for (int i = population.size(); i < populationSize; i++)
		{
			// TODO: this is not a probabilistically fair approach
			Genotype child = parents[0];

			for (size_t j = 1; j < parents.size(); j++)
			{
				child = mate(child, parents[j], prob);
			}

			//population.push_back(child);
			population.push_back(mutate(child));
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
	bool mutateGene = false;

	for (size_t i = 0; i < genotype.size(); i++)
	{
		if (groupGenes)
		{
			// group mutation
			if(i % groupSize == 0)
				mutateGene = ofRandom(1) < mutationRate;
		}
		else
		{	
			// mutate this gene with probability equal to the mutation rate
			mutateGene = ofRandom(1) < mutationRate;
		}
		
		
		if (mutateGene)
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
Genotype GeneticAlgorithm::mate(Genotype parent1, Genotype parent2, float probability)
{
	Genotype child = parent1;

	bool useOtherGene = false;

	// 2^N possible values and randomly select a few to be in the new population, also add the original parents
	for (size_t i = 0; i < child.size(); i++)
	{
		if (groupGenes)
		{
			// use the same genes for the entire group
			if (i % groupSize == 0)
				useOtherGene = ofRandom(1) < probability;
		}
		else
		{
			// crossover genes independently
			useOtherGene = ofRandom(1) < probability;
		}

		//useOtherGene = ofRandom(1) < probability;

		if (useOtherGene)
			child[i] = parent2[i];
	}





	return child;
}

//--------------------------------------------------------------
float GeneticAlgorithm::absDifference(int index, Genotype target)
{
	if (index < 0 || index >= population.size() || target.size() != population[index].size())
		return -1;

	float accumulated = 0;

	for (size_t i = 0; i < target.size(); i++)
	{
		accumulated += abs(target[i] - population[index][i]);
	}

	return accumulated;
}