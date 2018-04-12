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
void InteriorEvolver::setup(int _tiles, ArchitectureProgram* _pProgram)
{
	// TODO: Allow multiple floor shapes

	// base setup
	Evolver::setup(_tiles, _pProgram);

	// calculate the number of rooms and splits
	rooms = pProgram->rooms.size();
	splits = rooms - 1;

	// evolve a binary tree with #rooms leafs and #splits interior nodes
	roomOptimizationAlgorithm.setup(80, splits * 2, 0.2f, 0.4f);
	
	// TODO: initialize properly using leaves and splits
	geneticTreeAlgorithm.setup(tiles, splits + rooms);

	for (int i = 0; i < tiles; i++)
	{
		vector<InteriorRoom> interior = vector<InteriorRoom>();
		interiors.push_back(interior);

		trees.push_back(constructTestTree());
	}

	candidates.clear();

	// do first generation
	generate(vector<int>());
}

//--------------------------------------------------------------
void InteriorEvolver::setFloorShape(ofPolyline _floorshape)
{
	floorshape = _floorshape;
}

//--------------------------------------------------------------
vector<InteriorRoom>* InteriorEvolver::getInteriorAt(int tile)
{
	if (tile < candidates.size())
	{
		int index = candidates[tile];

		// select the indices in the genetic algorithm
		return &interiors[index];
	}

	return NULL;
}

//--------------------------------------------------------------
vector<InteriorRoom> InteriorEvolver::optimizeInterior(int treeIndex)
{
	// TODO: get split tree using treeIndex

	// for now just construct a balanced binary tree

	// generate the first population
	roomOptimizationAlgorithm.generateRandomPopulation();

	vector<float> fitnesses;
	vector<Split> splits;
	vector<Split> optimalSplits;

	// TODO: store all fitness values together with the splits in order to perform a better selection
	for (int gen = 0; gen <= optimizationGenerations; gen++)
	{
		// generate next generation
		roomOptimizationAlgorithm.generateOffspring();

		// convert
		float totalFitness = 0;
		fitnesses.clear();
		splits.clear();

		float maxfitness = -1.0f;
		int optimalIndex = -1;

		Genotype genotype;
		for (int i = 0; i < roomOptimizationAlgorithm.population.size(); i++)
		{	
			genotype = roomOptimizationAlgorithm.population[i];

			splits.clear();

			// create splits
			for (int j = 0; j < genotype.size(); j += 2)
			{
				splits.push_back(
					Split(
						genotype[j],								// position
						fminf(floorf(genotype[j+1] * 2.0), 1.0)		// axis
					)
				);	
			}

			// compute fitness
			float fitness = computeInteriorFitness(splits, treeIndex);
			totalFitness += fitness;
			fitnesses.push_back(fitness);

			// find the best best individual of the last generation

			if (fitness > maxfitness)
			{
				maxfitness = fitness;
				optimalIndex = i;

				if (gen == optimizationGenerations)
				{
					optimalSplits = splits;
				}
			}
		}

		if (treeIndex == 0)
		{
			cout << "gen: " << gen << ", total fitness: " << totalFitness << ", highest fitness:" << maxfitness << endl;
		}

		// TODO: select fittest 2 individuals
		//roomOptimizationAlgorithm.select(optimalIndex);

		roomOptimizationAlgorithm.selectByFitness(fitnesses);
	}

	// done with evolution, pick the genome with the highest fitness as our optimal room division
	//SplitTreeNode* root = constructTestTree();
	
	// create phenotype
	vector<InteriorRoom> interior = generateRooms(optimalSplits, trees[treeIndex], floorshape);

	return interior;
}


//--------------------------------------------------------------
float InteriorEvolver::computeInteriorFitness(const vector<Split>& splits, int treeIndex)
{
	float fitness = 0;

	SplitTreeNode* root = trees[treeIndex];
		//constructTestTree();

	// recursively subdivide polygon into rooms
	vector<InteriorRoom> rooms = generateRooms(splits, root, floorshape);

	// check room sizes
	for (int i = 0; i < rooms.size(); i++)
	{
		// TODO: should be proportional
		float areaDiff = abs(rooms[i].getArea() - rooms[i].pRoom->area);
		fitness += ofClamp(10.0f - (areaDiff / rooms[i].pRoom->area) * 10.0f, 0, 10);
	}

	// check room ratio 
	for (int i = 0; i < rooms.size(); i++)
	{
		float minDim = rooms[i].getMinDim();
		float maxDim = rooms[i].getMaxDim();

		float minDiff = abs(rooms[i].getMinDim() - rooms[i].pRoom->min);
		float maxDiff = abs(rooms[i].getMaxDim() - rooms[i].pRoom->max);

		if (rooms[i].pRoom->min <= minDim)
			fitness += 2;
		
		if (rooms[i].pRoom->max >= maxDim)
			fitness += 2;

		//fitness += ofClamp(10 - areaDiff, 0, 10);
	}

	// TODO: adjacency test

	return fitness;
}

//--------------------------------------------------------------
void InteriorEvolver::generate(vector<int> selection)
{
	// find selected indices
	for (int i = 0; i < selection.size(); i++)
	{
		// convert the tile indices to the indices used by the genetic algorithm
		int tile = selection[i];

		if (tile < candidates.size())
		{
			int index = candidates[tile];

			// select the indices in the genetic algorithm
			geneticTreeAlgorithm.select(index);
		}
	}

	candidates.clear();

	if (selection.size() > 0)
	{
		// let the genetic algorithm generate offspring based on the selection
		geneticTreeAlgorithm.generateOffspring();
	}
	else
	{
		geneticTreeAlgorithm.generateRandomPopulation();
		//setupEvolution();
	}

	if (floorshape.size() > 2)
	{
		// compute optimal interior division for these tree structures
		for (int i = 0; i < geneticTreeAlgorithm.population.size(); i++)
		{
			candidates.push_back(i);

			// optimize interiors
			//trees[i] = constructTestTree();
			interiors[i] = optimizeInterior(i);
		}

		//interiors[0] = optimizeInterior(0);
	}
}

// Recursive algorithm to construct interior rooms
//--------------------------------------------------------------
vector<InteriorRoom> InteriorEvolver::generateRooms(const vector<Split>& splits, SplitTreeNode* node, const ofPolyline& shape)
{
	vector<InteriorRoom> rooms;

	// recurse left child
	if(node->isLeaf())
	{
		// this is a leaf, add it to the list of rooms
		InteriorRoom ir = InteriorRoom(&pProgram->rooms[node->index], shape);
		rooms.push_back(ir);
	}
	else
	{
		// this is a split node, divide the polygon using the split information
		int si = node->index;

		// check if the index is valid, otherwise return
		if (si >= splits.size())
			return rooms;

		// compute left and right polygons
		ofPoint rayPos;
		ofVec2f rayDir;
		const ofRectangle* bb = &shape.getBoundingBox();

		// make sure the cut is not all the way at the start or end
		float pos = splits[si].position * 0.9f + 0.05f;

		// create split
		if (splits[si].axis == 0)
		{
			// x-axis
			rayPos = ofPoint(ofLerp(bb->getLeft(), bb->getRight(), pos), -10);
			rayDir = ofVec2f(0, 1);
		}
		else
		{
			// y-axis
			rayPos = ofPoint(-10, ofLerp(bb->getTop(), bb->getBottom(), pos));
			rayDir = ofVec2f(1, 0);
		}

		// split the current polygon in half
		ofPolyline leftShape;
		ofPolyline rightShape;
		IntersectionHelper::splitPolygon(shape, rayPos, rayDir, &leftShape, &rightShape);

		// continue recursion in left and right subtrees
		vector<InteriorRoom> roomsLeft = generateRooms(splits, node->leftChild, leftShape);
		vector<InteriorRoom> roomsRight = generateRooms(splits, node->rightChild, rightShape);

		// add results to the current 
		rooms.insert(rooms.end(), roomsLeft.begin(), roomsLeft.end());
		rooms.insert(rooms.end(), roomsRight.begin(), roomsRight.end());
	}

	return rooms;
}

//--------------------------------------------------------------
SplitTreeNode* InteriorEvolver::constructTestTree()
{
	SplitTreeNode* root = new SplitTreeNode(0);
	SplitTreeNode* lc = new SplitTreeNode(1);
	SplitTreeNode* rc = new SplitTreeNode(2);

	root->leftChild = lc;
	root->rightChild = rc;

	SplitTreeNode* lclc = new SplitTreeNode(3);
	SplitTreeNode* lcrc = new SplitTreeNode(0); // room 0

	lc->leftChild = lclc;
	lc->rightChild = lcrc;

	SplitTreeNode* rclc = new SplitTreeNode(1); // room 1
	SplitTreeNode* rcrc = new SplitTreeNode(2); // room 2

	rc->leftChild = rclc;
	rc->rightChild = rcrc;

	SplitTreeNode* lclclc = new SplitTreeNode(3); // room 3
	SplitTreeNode* lcrcrc = new SplitTreeNode(4); // room 4

	lclc->leftChild = lclclc;
	lclc->rightChild = lcrcrc;

	return root;
}