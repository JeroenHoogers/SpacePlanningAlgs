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

struct RoomEdge
{
	ofPoint p1;
	ofPoint p2;

	RoomEdge()
	{

	}

	RoomEdge(ofPoint _p1, ofPoint _p2)
	{
		p1 = _p1;
		p2 = _p2;
	}

	bool hasVertex(const ofPoint& p) const
	{
		float tau = 0.000001f;

		float d1 = p.distance(this->p1);
		float d2 = p.distance(this->p2);

		//float d1 =  (p, p1);
		//float d2 = Vector2.Distance(p, p2);

		return (d1 < tau || d2 < tau);

		//return (p1 == p || p2 == p);
	}

	ofPoint other(const ofPoint& p) const
	{
		float tau = 0.000001f;

		float d1 = p.distance(this->p1);
		float d2 = p.distance(this->p2);


		if (d1 < tau)
			return p2;

		if (d2 < tau)
			return p1;

		return ofPoint();
	}

	bool operator==(const RoomEdge& val) const
	{
		float tau = 0.000001f;

		float d1 = val.p1.distance(this->p1);
		float d2 = val.p2.distance(this->p2);

		float d3 = val.p2.distance(this->p1);
		float d4 = val.p1.distance(this->p2);

		return (d1 < tau && d2 < tau) || (d3 < tau && d4 < tau);
		//return (this->p1 == val.p1 && this->p2 == val.p2) || (this->p1 == val.p2 && this->p2 == val.p1);
	}

	bool operator<(const RoomEdge& val) const
	{
		if (this->p1.x != val.p1.x)
			return this->p1.x < val.p1.x;

		if (this->p1.y != val.p1.y)
			return this->p1.y < val.p1.y;

		if (this->p2.x != val.p2.x)
			return this->p2.x < val.p2.x;

		if (this->p2.y != val.p2.y)
			return this->p2.y < val.p2.y;


		/*ofPoint minP1 = this->p1;
		ofPoint maxP1 = this->p2;
		ofPoint minP2 = val.p1;
		ofPoint maxP2 = val.p2;

		if (this->p1.x > this->p2.x)
		{
			minP1 = this->p2;
			maxP1 = this->p1;
		}

		if (val.p1.x > val.p2.x)
		{
			minP2 = val.p2;
			maxP2 = val.p1;
		}

		if (minP1 != minP2)
		{
			if (minP1.x != minP2.x)
				return minP1.x < minP2.x;

			return minP1.y < minP2.y;
		}

		if (maxP1 != maxP2)
		{
			if (maxP1.x != maxP2.x)
				return maxP1.x < maxP2.x;

			return maxP1.y < maxP2.y;
		}*/

		//ofPoint& minP1 = (this->p1.x <= this->p2.x) ? this->p1 : this->p2;
		//ofPoint& minP2 = (val.p1.x <= val.p2.x) ? val.p1 : val.p2;


		//float c1 = fminf(this->p1.x, this->p2.x);
		//float c2 = fminf(val.p1.x, val.p2.x);

		//if (c1 != c2)
		//	return c1 < c2;

		//c1 = fminf(this->p1.y, this->p2.y);
		//c2 = fminf(val.p1.y, val.p2.y);

		//if (c1 != c2)
		//	return c1 < c2;


		//c1 = fmaxf(this->p1.x, this->p2.x);
		//c2 = fmaxf(val.p1.x, val.p2.x);

		//if (c1 != c2)
		//	return c1 < c2;

		//c1 = fmaxf(this->p1.y, this->p2.y);
		//c2 = fmaxf(val.p1.y, val.p2.y);

		//if (c1 != c2)
		//	return c1 < c2;

		// must be equal
		return false;
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
	int optimizationGenerations = 25;
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

	int m_selected = 0;

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

	void generateRooms(FloorGrid& floorgrid, vector<InteriorRoom>& rooms);

	void drawDebug(ofPoint p, int tile) override;

	int getSelectedTiles() override;
};

