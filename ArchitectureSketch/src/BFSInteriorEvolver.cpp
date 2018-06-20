#include "BFSInteriorEvolver.h"

//--------------------------------------------------------------
BFSInteriorEvolver::BFSInteriorEvolver()
{
}

//--------------------------------------------------------------
BFSInteriorEvolver::~BFSInteriorEvolver()
{
}

//--------------------------------------------------------------
void BFSInteriorEvolver::setup(int _tiles, ArchitectureProgram* _pProgram)
{
	// TODO: Allow multiple floor shapes
	// base setup
	Evolver::setup(_tiles, _pProgram);

	// calculate the number of rooms and splits
	nRooms = pProgram->rooms.size();
	optSplits = 3;

	// evolve a binary tree with #rooms leafs and #splits interior nodes
	roomOptimizationAlgorithm.setup(100, optSplits * 2, 0.1f, 0.2f);
	
	// initialize selection algorithm (stores room positions)
	selectionAlgorithm.setup(tiles, nRooms * 2);
	
	// initialize wall placement algorithm (depends on )
	int cells = (hSplits + 1) * (vSplits + 1);
	wallPlacementAlgorithm.setup(tiles, cells * 2);
	wallPlacementAlgorithm.randomBias = 0.1f;

	// distinct adjacency combinations (n-1) choose 2
	//int adjacencies = (nRooms * (nRooms - 1)) / 2;
	//adjacencyWeightsAlgorithm.setup(tiles, adjacencies);

	for (int i = 0; i < tiles; i++)
	{
		vector<InteriorRoom> interior = vector<InteriorRoom>();
		interiors.push_back(interior);

		floors.push_back(FloorGrid());
		gridTopologies.push_back(InteriorGrid(hSplits, vSplits));
		//trees.push_back(constructTestTree());
	}

	candidates.clear();

	// do first generation
	generate(vector<int>());
}

//--------------------------------------------------------------
void BFSInteriorEvolver::computeDefaultSplits()
{
	defaultSplits.clear();

	ofRectangle bb = floorshape.getBoundingBox();

	for (int i = 0; i < floorshape.size(); i++)
	{
		int prev = floorshape.getWrappedIndex(i - 1);
		int next = floorshape.getWrappedIndex(i + 1);

		ofVec2f v2 = floorshape[i] - floorshape[prev];
		ofVec2f v1 = floorshape[next] - floorshape[i];

		// reflex angle
		float angle = v1.angle(v2);
		if (angle < 0)
		{
			// create x-split
			defaultSplits.push_back(
				Split((floorshape[i].x - bb.getMinX()) / bb.getWidth(), 0)
			);

			// create y-split
			defaultSplits.push_back(
				Split((floorshape[i].y - bb.getMinY()) / bb.getHeight(), 1)
			);
		}
	}
}

// Constructs a rectangular grid based on the current floorshape and split positions
//--------------------------------------------------------------
FloorGrid BFSInteriorEvolver::constructGrid(const vector<Split>& splits)
{
	ofRectangle bb = floorshape.getBoundingBox();

	// get relevant floorshape 
	float minx = bb.getMinX();
	float miny = bb.getMinY();
	float w = bb.getWidth();
	float h = bb.getHeight();

	// divide floorshape into grid
	int rows = 1;
	int cols = 1;

	vector<float> xs;
	vector<float> ys;

	// add grid boundary positions
	xs.push_back(0);
	xs.push_back(1);

	ys.push_back(0);
	ys.push_back(1);

	vector<GridCell> cells;

	// determine grid dimensions
	for (int i = 0; i < splits.size(); i++)
	{
		if (splits[i].axis == 0)
		{
			cols++;
			xs.push_back(splits[i].position);
		}
		else if (splits[i].axis == 1)
		{
			rows++;
			ys.push_back(splits[i].position);
		}
	}

	// sort x and y positions
	sort(xs.begin(), xs.end());
	sort(ys.begin(), ys.end());

	for (int i = 0; i < ys.size() - 1; i++)
	{
		float cy = miny + ys[i] * h;
		float ch = (ys[i + 1] - ys[i]) * h;

		for (int j = 0; j < xs.size() - 1; j++)
		{
			float cx = minx + xs[j] * w;
			float cw = (xs[j + 1] - xs[j]) * w;

			// construct rectangle
			ofRectangle rect = ofRectangle(cx, cy, cw, ch);

			// check if this rectangle is part of the floorplan
			bool contained = floorshape.inside(rect.getCenter());

			// add cell to grid
			cells.push_back(GridCell(rect, contained));
		}
	}

	FloorGrid grid = FloorGrid(rows, cols, cells);
	return grid;
}

//--------------------------------------------------------------
void BFSInteriorEvolver::generateGridTopology(const vector<float>& positions, const vector<bool>& walls, FloorGrid* floorgrid)
{
	ofRectangle bb = floorshape.getBoundingBox();

	// get relevant floorshape 
	float minx = bb.getMinX();
	float miny = bb.getMinY();
	float w = bb.getWidth();
	float h = bb.getHeight();

	int cols = floorgrid->cols;
	int rows = floorgrid->rows;

	if (positions.size() != nRooms * 2)
		return;

	InteriorGrid topologyGrid = InteriorGrid(hSplits, vSplits);

	// generate graph with edges based on walls
	GridGraph graph = topologyGrid.GenerateGraph(walls);

	vector<ofPoint> roomCenters;
	vector<int> roomCells;
	floorgrid->areas.clear();

	for (int i = 0; i < positions.size(); i+=2)
	{
		ofPoint p = ofPoint(positions[i], positions[i + 1]);
		
		//ofPoint p = ofPoint(
		//	fminf(floorf(genotype[i] * cols), cols - 1),
		//	fminf(floorf(genotype[i+1] * rows), rows - 1)
		//);

		//ofPoint p = ofPoint(
		//	genotype[i] * cols,
		//	genotype[i + 1] * rows
		//);

		//ofPoint p = ofPoint(minx + genotype[i] * w, miny + genotype[i+1] * h);

		floorgrid->areas.push_back(0);
		floorgrid->centroids.push_back(ofPoint());

		roomCells.push_back(0);
		roomCenters.push_back(p);
	}

	// generate rooms based using centroids and BFS
	vector<map<int,int>> roomDistances = topologyGrid.GenerateRooms(graph, pProgram->rooms, roomCenters);

	int level = 0;

	// loop each cell
	for (int i = 0; i < floorgrid->cells.size(); i++)
	{
		//for (int j = 0; j < rows; j++)
		//{
		//ofPoint p = floorgrid->cells[i].rect.getCenter();
		//p = ofPoint(i % rows, floorf((float)i / (float)rows));

		int minlevel = 999999;
		int roomIndex = -1;

		// iterate over room centerpoints to find the closest one
		for (int k = 0; k < roomDistances.size(); k++)
		{
			// TODO: consider weights
			//float w = 1.0f - pProgram->getRoomAreaRatio(k);
			map<int, int>::iterator it = roomDistances[k].find(i);
			if (it != roomDistances[k].end())
			{
				level = roomDistances[k][i];
				if (level < minlevel)
				{
					minlevel = level;
					roomIndex = k;
				}
			}
		}

		// TODO: inside check is not always accurate in case of non-monotone polygons
		//floorgrid->cells[i].inside
		if (floorgrid->cells[i].inside && roomIndex >= 0)
		{
			floorgrid->areas[roomIndex] += floorgrid->cells[i].rect.getArea();
			floorgrid->centroids[roomIndex] += floorgrid->cells[i].rect.getCenter();
			roomCells[roomIndex]++;
		}

		floorgrid->cells[i].roomId = roomIndex;
		// set room index based on closest centerpoint
		//floorgrid->getCellAt(i, j).roomId = roomIndex;
		//}
	}


	//for (int i = 0; i < roomCells.size(); i++)
	//{
	//	// room has no cells
	//	if (roomCells[i] <= 0)
	//		floorgrid->centroids[i] = roomCenters[i];
	//	else
	//	{
	//		floorgrid->centroids[i] /= (float)roomCells[i];
	//	}
	//}

	vector<ofPoint> roomCentroids;
	for (int i = 0; i < roomCenters.size(); i++)
	{
		ofPoint pos = topologyGrid.getCellInterpolated(roomCenters[i]);
		roomCentroids.push_back(floorgrid->getCellAt((int)pos.x, (int)pos.y).rect.getCenter());
	}

	floorgrid->centers = roomCentroids;
}

//--------------------------------------------------------------
void BFSInteriorEvolver::setFloorShape(ofPolyline _floorshape)
{
	floorshape = _floorshape;

	// compute default splits
	computeDefaultSplits();

	vSplits = 0;
	hSplits = 0;

	for (auto& split : defaultSplits)
	{
		if (split.axis == 0)
			vSplits++;
		else
			hSplits++;
	}
	
	vSplits += optSplits;
	hSplits += optSplits;

	// re-initialize wall algorithm with cells * 2 variables
	int cells = (hSplits + 1) * (vSplits + 1);
	wallPlacementAlgorithm.setup(tiles, cells * 2);
}

//--------------------------------------------------------------
vector<InteriorRoom>* BFSInteriorEvolver::getInteriorAt(int tile)
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
vector<InteriorRoom> BFSInteriorEvolver::optimizeInterior(int index)
{
	// TODO: get split tree using treeIndex
	// for now just construct a balanced binary tree

	//if (gen == 0)
	//{
		// generate the first population
	roomOptimizationAlgorithm.generateRandomPopulation();
	//}
	//vector<float> fitnesses;
	vector<Split> splits;
	vector<Split> optimalSplits;

	// TODO: store all fitness values together with the splits in order to perform a better selection
	for (int gen = 0; gen <= optimizationGenerations; gen++)
	{
		// generate next generation
		roomOptimizationAlgorithm.generateOffspring();

		// convert
		float totalFitness = 0;
		//fitnesses.clear();
		splits.clear();

		float maxfitness = -1.0f;
		int optimalIndex = -1;

		Genotype<float>* pGenotype;
		for (int i = 0; i < roomOptimizationAlgorithm.population.size(); i++)
		{	
			pGenotype = &roomOptimizationAlgorithm.population[i];

			splits.clear();

			// create splits
			for (int j = 0; j < pGenotype->genes.size(); j++)
			{
				splits.push_back(
					Split(
						pGenotype->genes[j],		// position
						(j < optSplits) ? 0 : 1		// axis
					//	fminf(floorf(pGenotype->genes[j] * 2.0), 1.0)		
					)
				);	
			}


			// TODO: can be heavily optimized since it only needs to be performed once

			// create grid layout, by appending default splits to the dynamic splits
			splits.insert(splits.end(), defaultSplits.begin(), defaultSplits.end());
			FloorGrid floorGrid = constructGrid(splits);

			generateGridTopology(selectionAlgorithm.population[index].genes, wallPlacementAlgorithm.population[index].genes, &floorGrid);

			// compute fitness
			float fitness = computeInteriorFitness(floorGrid);
			totalFitness += fitness;

			pGenotype->fitness = fitness;

			// find the best best individual of the last generation
			if (fitness > maxfitness)
			{
				maxfitness = fitness;

				optimalIndex = i;
				optimalSplits = splits;
			}
		}

		if (index == 0)
		{
			cout << "gen: " << gen << ", total fitness: " << totalFitness << ", highest fitness:" << maxfitness << endl;
		}

		// TODO: select fittest 2 individuals
		//roomOptimizationAlgorithm.select(optimalIndex);

		//roomOptimizationAlgorithm.selectByFitness(fitnesses);
	}

	// create phenotype
	vector<InteriorRoom> interior;

	// TODO: generate rooms

	// create grid layout, by appending default splits to the dynamic splits
	floors[index] = constructGrid(optimalSplits);
	generateGridTopology(selectionAlgorithm.population[index].genes, wallPlacementAlgorithm.population[index].genes, &floors[index]);

//	generateRooms(optimalSplits, trees[treeIndex], floorshape, interior);

	return interior;
}

//--------------------------------------------------------------
float BFSInteriorEvolver::computeInteriorFitness(const FloorGrid& floorgrid)
{
	float areaFitness = 0;
	float ratioFitness = 0;
	float adjFitness = 0;

	//SplitTreeNode* root = trees[treeIndex];
		//constructTestTree();

	// recursively subdivide polygon into rooms

	// TODO: create polygon rooms

	//vector<InteriorRoom> rooms;
	//generateRooms(splits, root, floorshape, rooms);
	

	float areaRatio = abs(floorshape.getArea()) / pProgram->getTotalRoomArea();


	// compute room area fitness
	for (int i = 0; i < nRooms; i++)
	{
		float area = floorgrid.areas[i];

		//// sum the area of all rectangles
		//for (int j = 0; j < floorgrid.cells.size(); j++)
		//{
		//	if (floorgrid.cells[j].inside && floorgrid.cells[j].roomId == i)
		//		area += floorgrid.cells[j].rect.getArea();
		//}


		float areaDiff = abs(area - pProgram->rooms[i].area * areaRatio);
		areaFitness += ofClamp(10.0f - (areaDiff / pProgram->rooms[i].area) * 10.0f, 0, 10);

		//// TODO: should be proportional
		//float areaDiff = abs(rooms[i].getArea() - rooms[i].pRoom->area);
		//areaFitness += ofClamp(10.0f - (areaDiff / rooms[i].pRoom->area) * 10.0f, 0, 10);
	}

	// check room 

	// check room ratio 
	//for (int i = 0; i < rooms.size(); i++)
	//{
	//	float minDim = rooms[i].getMinDim();
	//	float maxDim = rooms[i].getMaxDim();

	//	float minDiff = abs(rooms[i].getMinDim() - rooms[i].pRoom->min);
	//	float maxDiff = abs(rooms[i].getMaxDim() - rooms[i].pRoom->max);

	//	if (rooms[i].pRoom->min <= minDim)
	//		ratioFitness += 3;
	//	
	//	if (rooms[i].pRoom->max >= maxDim)
	//		ratioFitness += 3;

	//	//fitness += ofClamp(10 - areaDiff, 0, 10);
	//}

	// obtain adjacency weights
	//DNA adjWeights = adjacencyWeightsAlgorithm.population[treeIndex].genes;
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
bool BFSInteriorEvolver::checkAdjacency(const InteriorRoom& r1, const InteriorRoom& r2)
{
	// TODO: should work for all shapes
	ofRectangle bb1 = r1.shape.getBoundingBox();
	ofRectangle bb2 = r2.shape.getBoundingBox();

	bool xOverlap = fabsf(bb2.getCenter().x - bb1.getCenter().x) <= (bb2.getWidth() + bb1.getWidth()) * 0.5f;
	bool yOverlap = fabsf(bb2.getCenter().y - bb1.getCenter().y) <= (bb2.getHeight() + bb1.getHeight()) * 0.5f;

	return xOverlap && yOverlap;
}

//--------------------------------------------------------------
void BFSInteriorEvolver::generate(vector<int> selection)
{
	// find selected indices
	for (int i = 0; i < selection.size(); i++)
	{
		// convert the tile indices to the indices used by the genetic algorithm
		int tile = selection[i];

		//if (tile < candidates.size())
		//{
		//	int index = candidates[tile];

			// select the indices in the genetic algorithm
			selectionAlgorithm.select(tile);
			//adjacencyWeightsAlgorithm.select(index);
		//}
	}

	candidates.clear();

	if (selection.size() > 0)
	{
		// let the genetic algorithm generate offspring based on the selection
		selectionAlgorithm.generateOffspring();
		wallPlacementAlgorithm.generateOffspring();
		//adjacencyWeightsAlgorithm.generateOffspring();
	}
	else
	{
		selectionAlgorithm.generateRandomPopulation();
		wallPlacementAlgorithm.generateRandomPopulation();
		//adjacencyWeightsAlgorithm.generateRandomPopulation();

		generation = 0;
		//setupEvolution();
	}

	if (floorshape.size() > 2)
	{
		// compute optimal interior division for these tree structures
		for (int i = 0; i < selectionAlgorithm.population.size(); i++)
		{
			candidates.push_back(i);

			// optimize interiors
			//trees[i] = constructTestTree();
			interiors[i] = optimizeInterior(i);
		}

		//interiors[0] = optimizeInterior(0);
	}
	generation++;
}

//--------------------------------------------------------------
void BFSInteriorEvolver::drawDebug(ofPoint p, int tile)
{
	if (tile >= selectionAlgorithm.population.size())
		return;

	string debugString = "";

	//DNA adjWeights = adjacencyWeightsAlgorithm.population[tile].genes;
	//int adjacencies = (nRooms * (nRooms - 1)) / 2;
	
	//for (int i = 0; i < nRooms; i++)
	//{
	//	float area = roundf(interiors[tile][i].getArea() * 10.0f) / 10.0f;
	//	debugString += interiors[tile][i].pRoom->code + " \t t: " + ofToString(interiors[tile][i].pRoom->area) + " m2, \t a: " + ofToString(area) + " m2\n";
	//}

	for (int i = 0; i < nRooms; i++)
	{
		float area = roundf(floors[tile].areas[i] * 10.0f) / 10.0f;
		debugString += pProgram->rooms[i].code + " \t t: " + ofToString(pProgram->rooms[i].area) + " m2, \t a: " + ofToString(area) + " m2\n";
	}

	//
	//for (int i = 0; i < floors[tile].rows; i++)
	//{
	//	for (int j = 0; j < floors[tile].cols; j++)
	//	{
	//		debugString += ofToString(floors[tile].getCellAt(j, i).roomId);
	//	}
	//	debugString += "\n";
	//}

	debugString += "\n";

	//for (int i = 0; i < nRooms - 1; i++)
	//{
	//	for (int j = i + 1; j < nRooms; j++)
	//	{
	//		// i is the minimum index
	//		int n = nRooms - i;
	//		int offset = adjacencies - (n * (n - 1)) / 2;
	//		int offset2 = j - i - 1;

	//		float w = roundf(adjWeights[offset + offset2] * 1000.0f) / 1000.0f;

	//		bool adjacent = false;
	//		if (interiors.size() > tile && interiors[tile].size() >= nRooms)
	//		{
	//			adjacent = checkAdjacency(interiors[tile][i], interiors[tile][j]);

	//			debugString += " (" + interiors[tile][i].pRoom->code + "," + interiors[tile][j].pRoom->code + "): \t" + ofToString(w) + "\t" + ofToString(adjacent) + "\n";
	//		}
	//		//adjacent = checkAdjacency(rooms[i], rooms[j]);
	//	}
	//}

	// draw adjacencies for current tile
	ofDrawBitmapStringHighlight(debugString, p);
}

//// Recursive algorithm to construct interior rooms
////--------------------------------------------------------------
//void InteriorEvolver::generateRooms(const vector<Split>& splits, const SplitTreeNode* node, const ofPolyline& shape, vector<InteriorRoom>& rooms)
//{
//	//vector<InteriorRoom> rooms;
//
//	// recurse left child
//	if (node->isLeaf())
//	{
//		// this is a leaf, add it to the list of rooms
//		InteriorRoom ir = InteriorRoom(&pProgram->rooms[node->index], shape);
//		rooms.push_back(ir);
//	}
//	else
//	{
//		// this is a split node, divide the polygon using the split information
//		int si = node->index;
//
//		// check if the index is valid, otherwise return
//		if (si >= splits.size())
//			return;
//
//		// compute left and right polygons
//		ofPoint rayPos;
//		ofVec2f rayDir;
//
//		const ofRectangle* bb = &shape.getBoundingBox();
//
//		// make sure the cut is not all the way at the start or end
//		float pos = splits[si].position * 0.8f + 0.1f;
//
//		// create split
//		if (splits[si].axis == 0)
//		{
//			// x-axis
//			rayPos = ofPoint(ofLerp(bb->getLeft(), bb->getRight(), pos), -10);
//			rayDir = ofVec2f(0, 1);
//		}
//		else
//		{
//			// y-axis
//			rayPos = ofPoint(-10, ofLerp(bb->getTop(), bb->getBottom(), pos));
//			rayDir = ofVec2f(1, 0);
//		}
//
//		// split the current polygon in half
//		ofPolyline leftShape;
//		ofPolyline rightShape;
//		IntersectionHelper::splitPolygon(shape, rayPos, rayDir, &leftShape, &rightShape);
//
//		// continue recursion in left and right subtrees
//	//	vector<InteriorRoom> roomsLeft = generateRooms(splits, node->leftChild, leftShape);
//	//	vector<InteriorRoom> roomsRight = generateRooms(splits, node->rightChild, rightShape);
//
//		generateRooms(splits, node->leftChild, leftShape, rooms);
//		generateRooms(splits, node->rightChild, rightShape, rooms);
//
//		//// add results to the current 
//		//rooms.insert(rooms.end(), roomsLeft.begin(), roomsLeft.end());
//		//rooms.insert(rooms.end(), roomsRight.begin(), roomsRight.end());
//	}
//}