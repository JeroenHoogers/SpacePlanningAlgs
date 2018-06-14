#pragma once
#include "GeneticAlgorithm.h"
#include "Evolver.h"
#include "IntersectionHelper.h"
#include "InteriorGrid.h"
#include "SplitInteriorEvolver.h"

//struct Split
//{
//	float position;
//
//	// 0: x-axis, 1: y-axis
//	int axis;
//
//	Split(float _position, int _axis)
//	{
//		position = _position;
//		axis = _axis;
//	}
//};

struct GridCell
{
	ofRectangle rect;
	int roomId;
	bool inside;

	GridCell()
	{

	}

	GridCell(ofRectangle _rect, bool _inside)
	{
		rect = _rect;
		inside = _inside;
	}
};

struct FloorGrid
{
	int rows = 0;
	int cols = 0;

	vector<GridCell> cells;
	vector<ofPoint> centers;
	vector<ofPoint> centroids;
	vector<float> areas;

	FloorGrid()
	{

	}

	FloorGrid(int _rows, int _cols, vector<GridCell> _cells)
	{
		rows = _rows;
		cols = _cols;
		cells = _cells;
	}

	GridCell& getCellAt(int x, int y)
	{
		if (x < 0 || x >= cols || y < 0 || y >= rows)
			return GridCell();

		return cells[x + y * cols];
	}
};

//// Room data structure
//struct InteriorRoom
//{
//	Room* pRoom;
//	ofPolyline shape;
//	ofRectangle bb;
//
//	InteriorRoom(Room* _pRoom, ofPolyline _shape)
//	{
//		pRoom = _pRoom;
//		shape = _shape;
//		bb = shape.getBoundingBox();
//	}
//
//	float getArea() const
//	{
//		return abs(shape.getArea());
//	}
//
//	float getMinDim() const
//	{
//		return fminf(bb.getWidth(), bb.getHeight());
//	}
//
//	float getMaxDim() const
//	{
//		return fmaxf(bb.getWidth(), bb.getHeight());
//	}
//};


// This is the layer between the genetic algorithm and the architecture state
class BFSInteriorEvolver : public Evolver
{
private:

	// amount of splits in the binary tree
	int optSplits = 0;

	int hSplits = 3;
	int vSplits = 3;

	// amount of rooms (leaves in the binary tree)
	int nRooms;

	// number of generations used by the room sizing algorithm
	int optimizationGenerations = 50;
	//int gen = 0;

	// total number of splits including 
	// TODO: change this to additional splits?
	int totalSplits = 8;

	GeneticAlgorithm<float> roomOptimizationAlgorithm;

	//GeneticTreeAlgorithm geneticTreeAlgorithm;
	//GeneticAlgorithm adjacencyWeightsAlgorithm;

	// use this for tree structure evolution
	//vector<> ;

	vector<Split> defaultSplits;

	vector<vector<InteriorRoom>> interiors;


public:

	vector<InteriorGrid> gridTopologies;

	// TODO: move back to private
	GeneticAlgorithm<float> selectionAlgorithm;
	GeneticAlgorithm<bool> wallPlacementAlgorithm;

	// TODO: allow multiple floors
	ofPolyline floorshape;

	// TEMP
	vector<FloorGrid> floors;

	BFSInteriorEvolver();
	~BFSInteriorEvolver();

	void setup(int _tiles, ArchitectureProgram* _pProgram);
	void setFloorShape(ofPolyline _floorshape);

	void computeDefaultSplits();

	FloorGrid constructGrid(const vector<Split>& splits);

	void generateGridTopology(const vector<float>& positions, const vector<bool>& walls, FloorGrid* floorgrid);

	vector<InteriorRoom>* getInteriorAt(int tile);

	void generate(vector<int> selection);

	vector<InteriorRoom> optimizeInterior(int index);

	float computeInteriorFitness(const FloorGrid& floorgrid);
	bool checkAdjacency(const InteriorRoom& r1, const InteriorRoom& r2);

	//void generateRooms(const vector<Split>& splits, const SplitTreeNode* node, const ofPolyline& shape, vector<InteriorRoom>& rooms);

	void drawDebug(ofPoint p, int tile) override;
};

