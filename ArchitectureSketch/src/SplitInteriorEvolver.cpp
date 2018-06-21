#include "SplitInteriorEvolver.h"

//--------------------------------------------------------------
SplitInteriorEvolver::SplitInteriorEvolver()
{
}

//--------------------------------------------------------------
SplitInteriorEvolver::~SplitInteriorEvolver()
{
}

//--------------------------------------------------------------
void SplitInteriorEvolver::setup(int _tiles, ArchitectureProgram* _pProgram)
{
	// TODO: Allow multiple floor shapes
	// base setup
	Evolver::setup(_tiles, _pProgram);

	// calculate the number of rooms and splits
	nRooms = pProgram->rooms.size();
	splits = nRooms - 1;

	// evolve a binary tree with #rooms leafs and #splits interior nodes
	roomOptimizationAlgorithm.setup(100, splits * 2, 0.1f, 0.4f);
	
	// TODO: initialize properly using leaves and splits
	geneticTreeAlgorithm.setup(tiles, splits + nRooms);

	// distinct adjacency combinations (n-1) choose 2
	int adjacencies = (nRooms * (nRooms - 1)) / 2;
	adjacencyWeightsAlgorithm.setup(tiles, adjacencies);

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
void SplitInteriorEvolver::setFloorShape(ofPolyline _floorshape)
{
	floorshape = _floorshape;
}

//--------------------------------------------------------------
vector<InteriorRoom>* SplitInteriorEvolver::getInteriorAt(int tile)
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
void SplitInteriorEvolver::optimizeInteriors()
{
	//if (gen >= optimizationGenerations)
	//	return;

	//// compute optimal interior division for these tree structures
	//for (int i = 0; i < geneticTreeAlgorithm.population.size(); i++)
	//{
	//	// optimize interiors
	//	interiors[i] = optimizeInterior(i);

	//}

	//gen++;
}

//--------------------------------------------------------------
vector<InteriorRoom> SplitInteriorEvolver::optimizeInterior(int treeIndex)
{
	// TODO: get split tree using treeIndex
	// for now just construct a balanced binary tree

	//if (gen == 0)
	//{
		// generate the first population
		roomOptimizationAlgorithm.generateRandomPopulation();
	//}
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

		Genotype<float>* pGenotype;
		for (int i = 0; i < roomOptimizationAlgorithm.population.size(); i++)
		{	
			pGenotype = &roomOptimizationAlgorithm.population[i];

			splits.clear();

			// create splits
			for (int j = 0; j < pGenotype->genes.size(); j += 2)
			{
				splits.push_back(
					Split(
						pGenotype->genes[j],								// position
						fminf(floorf(pGenotype->genes[j+1] * 2.0), 1.0)		// axis
					)
				);	
			}

			// compute fitness
			float fitness = computeInteriorFitness(splits, treeIndex);
			totalFitness += fitness;

			pGenotype->fitness = fitness;

//			fitnesses.push_back(fitness);

			// find the best best individual of the last generation
			if (fitness > maxfitness)
			{
				maxfitness = fitness;

				optimalIndex = i;
				optimalSplits = splits;
			}
		}

		//if (treeIndex == 0)
		//{
		//	cout << "gen: " << gen << ", total fitness: " << totalFitness << ", highest fitness:" << maxfitness << endl;
		//}

		// TODO: select fittest 2 individuals
		//roomOptimizationAlgorithm.select(optimalIndex);

		//roomOptimizationAlgorithm.selectByFitness(fitnesses);
	}

	// done with evolution, pick the genome with the highest fitness as our optimal room division
	//SplitTreeNode* root = constructTestTree();
	
	// create phenotype
	vector<InteriorRoom> interior;
	generateRooms(optimalSplits, trees[treeIndex], floorshape, interior);

	return interior;
}


//--------------------------------------------------------------
float SplitInteriorEvolver::computeInteriorFitness(const vector<Split>& splits, int treeIndex)
{
	float areaFitness = 0;
	float ratioFitness = 0;
	float adjFitness = 0;

	SplitTreeNode* root = trees[treeIndex];
		//constructTestTree();

	// recursively subdivide polygon into rooms
	vector<InteriorRoom> rooms;
	generateRooms(splits, root, floorshape, rooms);

	float areaRatio = abs(floorshape.getArea()) / pProgram->getTotalRoomArea();

	// check room sizes
	for (int i = 0; i < rooms.size(); i++)
	{
		// TODO: should be proportional
		float areaDiff = abs(rooms[i].getArea() - rooms[i].pRoom->area * areaRatio);
		areaFitness += ofClamp(10.0f - (areaDiff / rooms[i].pRoom->area) * 10.0f, 0, 10);
	}

	// check room ratio 
	for (int i = 0; i < rooms.size(); i++)
	{
		float minDim = rooms[i].getMinDim();
		float maxDim = rooms[i].getMaxDim();

		float minDiff = abs(rooms[i].getMinDim() - rooms[i].pRoom->min);
		float maxDiff = abs(rooms[i].getMaxDim() - rooms[i].pRoom->max);

		if (rooms[i].pRoom->min <= minDim)
			ratioFitness += 3;
		
		if (rooms[i].pRoom->max >= maxDim)
			ratioFitness += 3;

		//fitness += ofClamp(10 - areaDiff, 0, 10);
	}

	//// obtain adjacency weights
	//vector<float> adjWeights = adjacencyWeightsAlgorithm.population[treeIndex].genes;
	//int adjacencies = (nRooms * (nRooms - 1)) / 2;

	//// check all room combinations for adjacencies
	//for (int i = 0; i < rooms.size()-1; i++)
	//{
	//	for (int j = i+1; j < rooms.size(); j++)
	//	{
	//		// check if the rooms are adjacent
	//		if (checkAdjacency(rooms[i], rooms[j]))
	//		{
	//			// rooms are adjacent, add weight^2 * 10 to the fitness

	//			// i is the minimum index
	//			int n = nRooms - i;
	//			int offset = adjacencies - (n * (n - 1)) / 2;
	//			int offset2 = j - i - 1;

	//			float w = adjWeights[offset + offset2];

	//			// TODO: add adjacency weight to fitness function
	//			// square / cube weight to get proper influence 
	//			adjFitness += 10 * w * w;
	//		}
	//	}
	//}

	float fitness = areaFitness * 1 + ratioFitness * 1 + adjFitness * 1;

	return fitness;
}


//--------------------------------------------------------------
bool SplitInteriorEvolver::checkAdjacency(const InteriorRoom& r1, const InteriorRoom& r2)
{
	// TODO: should work for all shapes
	ofRectangle bb1 = r1.shape.getBoundingBox();
	ofRectangle bb2 = r2.shape.getBoundingBox();

	bool xOverlap = fabsf(bb2.getCenter().x - bb1.getCenter().x) <= (bb2.getWidth() + bb1.getWidth()) * 0.5f;
	bool yOverlap = fabsf(bb2.getCenter().y - bb1.getCenter().y) <= (bb2.getHeight() + bb1.getHeight()) * 0.5f;

	return xOverlap && yOverlap;
}

//--------------------------------------------------------------
void SplitInteriorEvolver::generate(vector<int> selection)
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
			adjacencyWeightsAlgorithm.select(index);
		}
	}

	candidates.clear();

	if (selection.size() > 0)
	{
		// let the genetic algorithm generate offspring based on the selection
		geneticTreeAlgorithm.generateOffspring();
		adjacencyWeightsAlgorithm.generateOffspring();
	}
	else
	{
		geneticTreeAlgorithm.generateRandomPopulation();
		adjacencyWeightsAlgorithm.generateRandomPopulation();

		generation = 0;
		//setupEvolution();
	}

	// check if the floorshape is valid
	if (floorshape.size() > 2)
	{
		// compute optimal interior division for these tree structures
		for (int i = 0; i < geneticTreeAlgorithm.population.size(); i++)
		{
			candidates.push_back(i);

			// construct tree using genome
			trees[i] = constructTree(geneticTreeAlgorithm.population[i].genes, 0, splits);

			if (i < 2)
				trees[i]->print();
			//trees[i] = constructTestTree();

			orderLeaves(trees[i]);

			if (i < 2)
				trees[i]->print();
			
			// optimize interiors
			interiors[i] = optimizeInterior(i);
		}

		//cout << "------------------------" << endl;
		//SplitTreeNode* tree = constructTestTree();
		//tree->print();
		//orderLeaves(tree);
		//tree->print();

		//interiors[0] = optimizeInterior(0);
	}
	generation++;
}

//--------------------------------------------------------------
void SplitInteriorEvolver::drawDebug(ofPoint p, int tile)
{
	if (tile >= adjacencyWeightsAlgorithm.population.size())
		return;

	string debugString = "";

	vector<float> adjWeights = adjacencyWeightsAlgorithm.population[tile].genes;
	int adjacencies = (nRooms * (nRooms - 1)) / 2;
	
	for (int i = 0; i < nRooms; i++)
	{
		float area = roundf(interiors[tile][i].getArea() * 10.0f) / 10.0f;
		debugString += interiors[tile][i].pRoom->code + " \t t: " + ofToString(interiors[tile][i].pRoom->area) + " m2, \t a: " + ofToString(area) + " m2\n";
	}

	debugString += "\n";

	for (int i = 0; i < nRooms - 1; i++)
	{
		for (int j = i + 1; j < nRooms; j++)
		{
			// i is the minimum index
			int n = nRooms - i;
			int offset = adjacencies - (n * (n - 1)) / 2;
			int offset2 = j - i - 1;

			float w = roundf(adjWeights[offset + offset2] * 1000.0f) / 1000.0f;

			bool adjacent = false;
			if (interiors.size() > tile && interiors[tile].size() >= nRooms)
			{
				adjacent = checkAdjacency(interiors[tile][i], interiors[tile][j]);

				debugString += " (" + interiors[tile][i].pRoom->code + "," + interiors[tile][j].pRoom->code + "): \t" + ofToString(w) + "\t" + ofToString(adjacent) + "\n";
			}
			//adjacent = checkAdjacency(rooms[i], rooms[j]);
		}
	}

	// draw adjacencies for current tile
	ofDrawBitmapStringHighlight(debugString, p);
}


// Recursive algorithm to construct interior rooms
//--------------------------------------------------------------
void SplitInteriorEvolver::generateRooms(const vector<Split>& splits, const SplitTreeNode* node, const ofPolyline& shape, vector<InteriorRoom>& rooms)
{
	//vector<InteriorRoom> rooms;

	// recurse left child
	if(node->isLeaf())
	{
		// this is a leaf, add it to the list of rooms
		// ERROR: Crash here
		if (node->index >= 0 && node->index < pProgram->rooms.size())
		{
			InteriorRoom ir = InteriorRoom(&pProgram->rooms[node->index], shape);
			rooms.push_back(ir);
		}
	}
	else
	{
		// this is a split node, divide the polygon using the split information
		int si = node->index;

		// check if the index is valid, otherwise return
		if (si >= splits.size())
			return;

		// compute left and right polygons
		ofPoint rayPos;
		ofVec2f rayDir;
		
		const ofRectangle* bb = &shape.getBoundingBox();

		// make sure the cut is not all the way at the start or end
		float pos = splits[si].position * 0.8f + 0.1f;

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
	//	vector<InteriorRoom> roomsLeft = generateRooms(splits, node->leftChild, leftShape);
	//	vector<InteriorRoom> roomsRight = generateRooms(splits, node->rightChild, rightShape);

		generateRooms(splits, node->leftChild, leftShape, rooms);
		generateRooms(splits, node->rightChild, rightShape, rooms);

		//// add results to the current 
		//rooms.insert(rooms.end(), roomsLeft.begin(), roomsLeft.end());
		//rooms.insert(rooms.end(), roomsRight.begin(), roomsRight.end());
	}
}
//--------------------------------------------------------------
SplitTreeNode* SplitInteriorEvolver::constructTree(const vector<float>& treeGenome, int i, int n)
{
	SplitTreeNode* root = new SplitTreeNode(i);

	SplitTreeNode* leftLeaf;
	SplitTreeNode* rightLeaf;

	// base case, construct node with 2 leaves
	if (n == 1)
	{
		leftLeaf = new SplitTreeNode(0); // room node
		rightLeaf = new SplitTreeNode(0); // room node
	}
	else // n >= 2
	{
		int nLeft = floorf(n * treeGenome[i]);		// nodes in left subtree
		int nRight = n - 1 - nLeft;					// nodes in right subtree

		if(nLeft > 0)
			leftLeaf = constructTree(treeGenome, i + 1, nLeft);
		else 
			leftLeaf = new SplitTreeNode(0); // room node

		if (nRight > 0)
			rightLeaf = constructTree(treeGenome, i + 1 + nLeft, nRight);
		else
			rightLeaf = new SplitTreeNode(0); // room node
	}

	// append childs to root
	root->leftChild = leftLeaf;
	root->rightChild = rightLeaf;

	return root;
}

//--------------------------------------------------------------
void SplitInteriorEvolver::orderLeaves(SplitTreeNode* root)
{
	int count = 0;
	stack<SplitTreeNode*> s;
	SplitTreeNode* current = root;
	
	bool done = false;

	// Do traversal
	while (!done)
	{
		if (current != NULL)
		{
			s.push(current);
			current = current->leftChild;
		}
		else
		{
			if (!s.empty())
			{
				current = s.top();

				// set index if the current node is a leaf
				if (current->isLeaf())
				{
					current->index = count;
					count++;
				}

				s.pop();

				// the left subtree has been completely visited, switch to the right subtree
				current = current->rightChild;
			}
			else
			{
				done = true;
			}
		}
	}

	//cout << count << endl;
}

//--------------------------------------------------------------
SplitTreeNode* SplitInteriorEvolver::constructTestTree()
{
	// TODO: generate this tree randomly
	SplitTreeNode* root = new SplitTreeNode(0);
	SplitTreeNode* lc = new SplitTreeNode(1);
	SplitTreeNode* rc = new SplitTreeNode(2);

	root->leftChild = lc;
	root->rightChild = rc;

	SplitTreeNode* lclc = new SplitTreeNode(3);
	SplitTreeNode* lcrc = new SplitTreeNode(0); // room 0

	lc->leftChild = lclc;
	lc->rightChild = lcrc;

	SplitTreeNode* rclc = new SplitTreeNode(0); // room 1
	SplitTreeNode* rcrc = new SplitTreeNode(0); // room 2

	rc->leftChild = rclc;
	rc->rightChild = rcrc;

	SplitTreeNode* lclclc = new SplitTreeNode(0); // room 3
	SplitTreeNode* lcrcrc = new SplitTreeNode(0); // room 4

	lclc->leftChild = lclclc;
	lclc->rightChild = lcrcrc;

	return root;
}

//--------------------------------------------------------------
SplitTreeNode* SplitInteriorEvolver::constructTestTree2()
{
	// TODO: generate this tree randomly
	SplitTreeNode* root = new SplitTreeNode(0);
	SplitTreeNode* lc = new SplitTreeNode(1);
	SplitTreeNode* rc = new SplitTreeNode(0); // room 0

	root->leftChild = lc;
	root->rightChild = rc;

	SplitTreeNode* lclc = new SplitTreeNode(2);
	SplitTreeNode* lcrc = new SplitTreeNode(3);

	lc->leftChild = lclc;
	lc->rightChild = lcrc;

	SplitTreeNode* lcrclc = new SplitTreeNode(1); // room 1
	SplitTreeNode* lcrcrc = new SplitTreeNode(2); // room 2

	lcrc->leftChild = lcrclc;
	lcrc->rightChild = lcrcrc;

	SplitTreeNode* lclclc = new SplitTreeNode(3); // room 3
	SplitTreeNode* lclcrc = new SplitTreeNode(4); // room 4

	lclc->leftChild = lclclc;
	lclc->rightChild = lclcrc;

	return root;
}