#include "GeneticAlgorithm.h"

//--------------------------------------------------------------
template<typename T>
GeneticAlgorithm<T>::GeneticAlgorithm()
{

}

//--------------------------------------------------------------
template<typename T>
GeneticAlgorithm<T>::~GeneticAlgorithm()
{

}

//--------------------------------------------------------------
template<typename T>
void GeneticAlgorithm<T>::setup(int popSize, int n)
{
	currentGeneration = 0; 

	populationSize = popSize;
	numGenes = n;

	matingStrat = EMatingStrategy::SwitchSource;
	
	// fill first gen with random data
	generateRandomPopulation();
}

//--------------------------------------------------------------
template<typename T>
void GeneticAlgorithm<T>::setup(int popSize, int n, float mutRate, float mutAmount)
{
	currentGeneration = 0;

	populationSize = popSize;
	numGenes = n;

	mutationRate = mutRate;
	mutationAmount = mutAmount;

	matingStrat = EMatingStrategy::SwitchSource;

	// fill first gen with random data
	generateRandomPopulation();
}

//--------------------------------------------------------------
template<typename T>
void GeneticAlgorithm<T>::generateRandomPopulation()
{
	population.clear();

	for (size_t i = 0; i < populationSize; i++)
	{
		population.push_back(Genotype<T>(generateRandomDna()));
	}
}

//--------------------------------------------------------------
template<typename T>
vector<T> GeneticAlgorithm<T>::generateRandomDna()
{
	vector<T> genes = vector<T>();
	for (size_t i = 0; i < numGenes; i++)
	{
		genes.push_back(T());
	}

	return genes;
}

//--------------------------------------------------------------
template<typename T>
void GeneticAlgorithm<T>::select(int index)
{
	// set fitness to 1
	population[index].fitness = 1;

	if(index >= 0 && index < population.size())
		selectedIndices.push_back(index);
}

//--------------------------------------------------------------
template<typename T>
void GeneticAlgorithm<T>::selectByFitness(const vector<float>& fitnesses)
{
	selectedIndices.clear();

	// calculate fitness sum
	float totalFitness = 0;
	float maxFitness = 0;
	int best = 0;

	for (int i = 0; i < fitnesses.size(); i++)
	{
		totalFitness += fitnesses[i];

		if (maxFitness < fitnesses[i])
		{
			maxFitness = fitnesses[i];
			best = i;
		}
	}

	selectedIndices.push_back(best);

	// refill population
	for (int i = 1; i < populationSize; i++)
	{
		// pick a member based on fitness
		float rand = ofRandom(totalFitness);

		float cumulativeFitness = 0;
		for (int j = 0; j < fitnesses.size(); j++)
		{
			cumulativeFitness += fitnesses[j];
			if (rand <= cumulativeFitness)
			{
				selectedIndices.push_back(j);
				break;
			}
		}
	}

	// pick new population based on fitness
	//population.clear();
}

//--------------------------------------------------------------
template<typename T>
void GeneticAlgorithm<T>::generateOffspring()
{
	vector<Genotype<T>> temp;

	// calculate fitness sum
	float totalFitness = 0;
	float maxFitness = 0;
	int fittestIndex = 0;

	for (int i = 0; i < population.size(); i++)
	{
		totalFitness += population[i].fitness;

		if (maxFitness < population[i].fitness)
		{
			maxFitness = population[i].fitness;
			fittestIndex = i;
		}
	}

	// TODO: select multiple individuals
	// TODO: different for selected
	// elitism, copy the best individuals of the population without any mutation
	if (elitism)
	{
		// selection based elitism
		if (selectedIndices.size() > 0)
		{
			for (size_t i = 0; i < selectedIndices.size(); i++)
			{
				temp.push_back(Genotype<T>(population[selectedIndices[i]].genes));
			}
		}
		else // automated
		{
			temp.push_back(Genotype<T>(population[fittestIndex].genes));
		}
	}
		
	// create new population
	while (temp.size() < population.size() - 1)
	{
		// select 2 members of the old population using roulette selection
		vector<T> offspring1 = rouletteSelection(totalFitness);
		vector<T> offspring2 = rouletteSelection(totalFitness);

		// crossover
		crossover(&offspring1, &offspring2);

		// mutate
		mutate(&offspring1);
		mutate(&offspring2);

		// add offspring to new generation
		temp.push_back(Genotype<T>(offspring1));
		temp.push_back(Genotype<T>(offspring2));
	}

	// in some cases where the population size is uneven, we need to add 1 extra individual
	if (temp.size() == population.size() - 1)
	{
		vector<T> offspring = rouletteSelection(totalFitness);
		mutate(&offspring);
		temp.push_back(Genotype<T>(offspring));
	}

	// replace population
	population.swap(temp);

	// remove selection
	selectedIndices.clear();

	// increase generation counter
	currentGeneration++;
}
//
//// TODO: crossover should be based on a probability
//// TODO: mutation should be based on a probability
////--------------------------------------------------------------
//void GeneticAlgorithm::generateOffspring()
//{
//	//// fitness based selection has been performed
//	//if (selectedIndices.size() == population.size())
//	//{
//	//	// TODO: could probably be made more efficient (vector reserve)
//	//	// store old population temporary
//	//	vector<Genotype> pop = population;
//
//	//	// clear population
//	//	//population.clear();
//
//	//	for (int i = 0; i < selectedIndices.size(); i++)
//	//	{	
//	//		// fill population
//	//		//population.push_back(pop[selectedIndices[i]]);
//
//	//		population[i] = pop[selectedIndices[i]];
//
//	//		// do mutation
//	//		population[i] = mutate(population[i]);
//	//	}
//	//}
//
//	if (selectedIndices.size() == 0) // no selection 
//	{
//		// clear population
//		population.clear();
//
//		// create new random population
//		generateRandomPopulation();
//	}
//	else if (selectedIndices.size() == 1) // mutate single 
//	{
//		// mutate single
//		Genotype selected = population[selectedIndices[0]];
//
//		// clear population
//		population.clear();
//
//		// add selected individual (optional)
//		if (elitism)
//		{
//			population.push_back(selected);
//		}
//
//		// populate the rest with mutations of the selected genotype
//		for (int i = population.size(); i < populationSize; i++)
//		{
//			mutate(&selected.genes);
//			population.push_back(selected);
//		}
//	}
//	else if (selectedIndices.size() == 2) // mate
//	{
//		// mate 2 parents
//		DNA parent1 = population[selectedIndices[0]].genes;
//		DNA parent2 = population[selectedIndices[1]].genes;
//
//		// clear population
//		population.clear();
//
//		// add selected individuals (optional)
//		if (elitism)
//		{
//			population.push_back(parent1);
//			population.push_back(parent2);
//		}
//
//		// populate the rest with mutations of the selected genotype
//		for (int i = population.size(); i < populationSize; i++)
//		{
//			DNA child;
//
//			switch (matingStrat)
//			{
//			default:
//			case EMatingStrategy::SwitchSource:
//				child = crossoverSwitchGenotype(parent1, parent2, 3);
//				break;
//			case EMatingStrategy::Gene:
//				child = crossover(parent1, parent2, 0.5f);
//				break;
//			case EMatingStrategy::Interpolate:
//				child = crossoverInterpolation(parent1, parent2);
//				break;
//			}
//
//			//Genotype child = crossover(parent1, parent2, 0.5f);
//			
//			//Genotype child = crossoverInterpolation(parent1, parent2);
//			mutate(&child);
//
//			population.push_back(Genotype(child));
//			
//		}
//	}
//	else if (selectedIndices.size() > 2)
//	{
//		vector<Genotype> parents;
//
//		for (int i = 0; i < selectedIndices.size(); i++)
//		{
//			parents.push_back(population[selectedIndices[i]]);
//		}
//
//		// clear population
//		population.clear();
//
//		// add selected individuals (optional)
//		if (elitism)
//		{
//			for (int i = 0; i < parents.size(); i++)
//			{
//				population.push_back(parents[i]);
//			}
//		}
//
//		float prob = 1.0f / selectedIndices.size();
//
//		// populate the rest with mutations of the selected genotype
//		for (int i = population.size(); i < populationSize; i++)
//		{
//			// TODO: this is not a probabilistically fair approach
//			DNA child = parents[0].genes;
//
//			for (size_t j = 1; j < parents.size(); j++)
//			{
//				child = crossover(child, parents[j], prob);
//			}
//
//			//population.push_back(child);
//			population.push_back(mutate(&child));
//		}
//	}
//	
//	// remove selection
//	selectedIndices.clear();
//
//	// increase generation counter
//	currentGeneration++;
//}


//--------------------------------------------------------------
template <>
vector<float> GeneticAlgorithm<float>::generateRandomDna()
{
	vector<float> genes = vector<float>();
	for (size_t i = 0; i < numGenes; i++)
	{
		genes.push_back(ofRandom(1));
	}

	return genes;
}


//--------------------------------------------------------------
template <>
vector<bool> GeneticAlgorithm<bool>::generateRandomDna()
{
	vector<bool> genes = vector<bool>();
	for (size_t i = 0; i < numGenes; i++)
	{
		genes.push_back(ofRandom(1) > 0.5f);
	}

	return genes;
}


//--------------------------------------------------------------
template <>
void GeneticAlgorithm<float>::mutate(vector<float>* genes)
{
	//vector<float> genotype;
	bool mutateGene = false;

	for (size_t i = 0; i < genes->size(); i++)
	{
		// mutate this gene with probability equal to the mutation rate
		mutateGene = ofRandom(1) < mutationRate;

		if (mutateGene)
		{
			// adjust 
			(*genes)[i] += ofRandom(-mutationAmount, mutationAmount);

			// TODO: implement different wrapping method to avoid values to get stuck at 0 or 1
			// clamp the mutation to a legal value
			(*genes)[i] = ofClamp((*genes)[i], 0, 1);
		}
	}
}

//--------------------------------------------------------------
template <>
void GeneticAlgorithm<bool>::mutate(vector<bool>* genes)
{
	//vector<float> genotype;
	bool mutateGene = false;

	for (size_t i = 0; i < genes->size(); i++)
	{
		// mutate this gene with probability equal to the mutation rate
		mutateGene = ofRandom(1) < mutationRate;

		if (mutateGene)
		{
			// negate gene 
			(*genes)[i] = !(*genes)[i];
		}
	}
}

//--------------------------------------------------------------
template<typename T>
void GeneticAlgorithm<T>::mutate(vector<T>* genes)
{
	////vector<float> genotype;
	//bool mutateGene = false;

	//for (size_t i = 0; i < genes->size(); i++)
	//{
	//	if (groupGenes)
	//	{
	//		// group mutation
	//		if(i % groupSize == 0)
	//			mutateGene = ofRandom(1) < mutationRate;
	//	}
	//	else
	//	{	
	//		// mutate this gene with probability equal to the mutation rate
	//		mutateGene = ofRandom(1) < mutationRate;
	//	}
	//	
	//	if (mutateGene)
	//	{
	//		// adjust 
	//		(*genes)[i] += ofRandom(-mutationAmount, mutationAmount);

	//		// TODO: implement different wrapping method to avoid values to get stuck at 0 or 1
	//		// clamp the mutation to a legal value
	//		(*genes)[i] = ofClamp((*genes)[i], 0, 1);
	//	}
	//}
}

//--------------------------------------------------------------
template<typename T>
vector<T> GeneticAlgorithm<T>::rouletteSelection(float totalFitness)
{
	// pick a member based on fitness
	float rand = ofRandom(totalFitness);

	float cumulativeFitness = 0.0f;

	for (int i = 0; i < population.size(); i++)
	{
		cumulativeFitness += population[i].fitness;

		if (rand <= cumulativeFitness)
		{
			return population[i].genes;
		}
	}

	return vector<T>();
}


//--------------------------------------------------------------
template<typename T>
void GeneticAlgorithm<T>::crossover(vector<T>* offspring1, vector<T>* offspring2)
{
	// crossover
	if (offspring1->size() != offspring2->size())
		return;

	for (size_t i = 0; i < offspring1->size(); i++)
	{
		// crossover genes independently
		//		useOtherGene = ofRandom(1) < probability;

		// crossover genes
		if (ofRandom(1) < 0.5f)
		{
			// do crossover
			// swap genes
			float v = (*offspring1)[i];
			(*offspring1)[i] = (*offspring2)[i];
			(*offspring2)[i] = v;
		}
	}
}

//
////--------------------------------------------------------------
//DNA GeneticAlgorithm::crossover(const DNA& parent1, const DNA& parent2, float probability)
//{
//	DNA child = parent1;
//
//	bool useOtherGene = false;
//
//	// 2^N possible values and randomly select a few to be in the new population, also add the original parents
//	for (size_t i = 0; i < child.size(); i++)
//	{
//		if (groupGenes)
//		{
//			// use the same genes for the entire group
//			if (i % groupSize == 0)
//				useOtherGene = ofRandom(1) < probability;
//		}
//		else
//		{
//			// crossover genes independently
//			useOtherGene = ofRandom(1) < probability;
//		}
//
//		//useOtherGene = ofRandom(1) < probability;
//
//		if (useOtherGene)
//			child[i] = parent2[i];
//	}
//
//	return child;
//}
//
////--------------------------------------------------------------
//DNA GeneticAlgorithm::crossoverInterpolation(const DNA& parent1, const DNA& parent2)
//{
//	DNA child = parent1;
//
//	float p = 0;
//
//	for (size_t i = 0; i < parent1.size(); i++)
//	{
//		// random percentage
//		p = ofRandom(1);
//		
//		// interpolate gene value
//		child[i] = parent1[i] * p + (1.0f - p) * parent2[i];
//	}
//
//	return child;
//}
//
////--------------------------------------------------------------
//DNA GeneticAlgorithm::crossoverSwitchGenotype(const DNA& parent1, const DNA& parent2, int frequency)
//{
//	DNA child = parent1;
//
//	vector<int> crossoverPoints = vector<int>();
//	bool useOtherGene = false;
//
//	// generate random crossover points
//	for (size_t i = 0; i < frequency; i++)
//	{
//		crossoverPoints.push_back((int)ofRandom(numGenes));
//	}
//
//	// sort in descending order
//	unique(crossoverPoints.begin(), crossoverPoints.end());
//	sort(crossoverPoints.begin(), crossoverPoints.end());
//	reverse(crossoverPoints.begin(), crossoverPoints.end());
//
//	for (size_t i = 0; i < parent1.size(); i++)
//	{
//		// crossover genes
//		if (crossoverPoints.size() >= 1 && i == crossoverPoints[crossoverPoints.size() - 1])
//		{
//			// do crossover
//			useOtherGene = !useOtherGene;
//			crossoverPoints.pop_back();
//		}
//	
//		if (useOtherGene)
//			child[i] = parent2[i];
//	}
//
//	return child;
//}

//
////--------------------------------------------------------------
//float GeneticAlgorithm::absDifference(int index, DNA target)
//{
//	if (index < 0 || index >= population.size() || target.size() != population[index].genes.size())
//		return -1;
//
//	float accumulated = 0;
//
//	for (size_t i = 0; i < target.size(); i++)
//	{
//		accumulated += abs(target[i] - population[index].genes[i]);
//	}
//
//	return accumulated;
//}


template class GeneticAlgorithm<float>;
template class GeneticAlgorithm<bool>;