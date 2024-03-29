#pragma once

#include "ofMain.h"
#include "GeneticAlgorithm.h"
#include "MeshHelper.h" 
#include "ArchitectureProgram.h"
#include "Measurements.h"
#include "StraightSkeleton.h"
#include "SplitInteriorEvolver.h"


enum struct ERoofType
{
	Flat,
	Hip,
	Gable
};

struct Subdivision
{
	Subdivision(float pos)
	{
		cutpos = pos;
	}

	float cutpos; // could be an interpolation along the entire shape
};

struct Extrusion
{
	// could be encoded in a single float where the interpolation variable t is derived from the local position on the line segment
	Extrusion(float pos, float amount, int floor, bool side, float order = 0)
	{
		position = pos;
		extrudeAmount = amount;
		extrudeFloor = floor;
		extrudeSide = side;
		
		ordering = order;
		//extrudeAngle = angle;
	}

	bool extrudeSide;
	float position;		// the face to extrude
	float extrudeAmount;	// the extrusion amount
	int extrudeFloor = -1;	// apply the extrusion to a specific floor (-1) applies it to all floors

	float ordering;
	//float extrudeAngle;			// angle of extrusion (limited between -45 and + 45 degrees) 
};

//struct Lin
//{
//	Line(ofPoint _p1, ofPoint _p2)
//	{
//		p1 = _p1;
//		p2 = _p2;
//	}
//
//	ofPoint p1;
//	ofPoint p2;
//};

class Building
{
private:
	int mFloors = 3;
	float floorHeight = 3.0f; // 3 m
	//float ceilingHeight = 0.3f; // 30 cm
	//float wallWidth = 0.2f; // 20 cm

	ERoofType mRoofType; // 0 for flat, 1 for hip
	float mRoofPitch;

	ofMesh buildingMesh;
	ofMesh roofMesh;

	ofMesh interiorMesh;

	vector<ofMesh> floorMeshes;

	vector<ofPolyline> lines;
	vector<ofPolyline> interiorLines;

	ofPolyline parcel;

	void generateFloorShapes();

	void extrudeShape(ofMesh& mesh, ofPolyline shape, ofVec3f bottomOffset, ofVec3f topOffset, bool ccw = true);

	void applyExtrusions(ofPolyline &floorshape, int floor);

	void generateMesh();

	void generateInteriorMesh(vector<InteriorRoom> rooms);

	void generateRoof();

public:
	bool hasInterior = false;

	ofRectangle boundingBox = ofRectangle(-10, -20, 20, 30);
	//vector<Subdivision> subdivs;
	vector<Extrusion> mExtrusions;

	vector<ofPolyline> floorShapes;

	//--------------------------------------------------------------
	Building()
	{
		buildingMesh = ofMesh();
	//	subdivs = vector<Subdivision>();
		mExtrusions = vector<Extrusion>();
	}

	//--------------------------------------------------------------
	~Building()
	{

	}

	//--------------------------------------------------------------
	const ofRectangle GetFloorBoundingBox(int index)
	{
		if (index >= 0 && index < floorShapes.size())
			return floorShapes[index].getBoundingBox();

		return ofRectangle();
	}

	//void LoadFromGenotype(vector<float> gt, ArchitectureProgram program = ArchitectureProgram());

	void Create(float width, float height, int floors, vector<Extrusion> extrusions, ERoofType roof = ERoofType::Flat, float roofPitch = 0);

	void SetInterior(vector<InteriorRoom> interior);
	
	void GenerateBuilding();

	float GetTotalArea();

	void draw(int floor = -1);
};