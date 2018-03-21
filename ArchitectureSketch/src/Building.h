#pragma once

#include "ofMain.h"
#include "GeneticAlgorithm.h"
#include "MeshHelper.h" 
#include "ArchitectureProgram.h"
#include "Measurements.h"

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
	Extrusion(float pos, float amount, int floor, float angle = 0)
	{
		position = pos;
		extrudeAmount = amount;
		extrudeFloor = floor;
		extrudeAngle = angle;
	}

	float position;		// the face to extrude
	float extrudeAmount;	// the extrusion amount
	int extrudeFloor = -1;	// apply the extrusion to a specific floor (-1) applies it to all floors

	float extrudeAngle;			// angle of extrusion (limited between -45 and + 45 degrees) 
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
	int floors = 3;
	float floorHeight = 3.0f; // 3 m
	//float ceilingHeight = 0.3f; // 30 cm
	//float wallWidth = 0.2f; // 20 cm

	ofMesh buildingMesh;

	vector<ofMesh> floorMeshes;

	vector<ofPolyline> lines;

	ofPolyline parcel;

	void generateFloorShapes();

	void applyExtrusions(ofPolyline &floorshape, int floor);

	void generateMesh();

public:
	ofRectangle boundingBox = ofRectangle(-10, -20, 20, 30);
	vector<Subdivision> subdivs;
	vector<Extrusion> extrusions;

	vector<ofPolyline> floorShapes;

	//--------------------------------------------------------------
	Building()
	{
		buildingMesh = ofMesh();
		subdivs = vector<Subdivision>();
		extrusions = vector<Extrusion>();
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

	void LoadFromGenotype(Genotype gt, ArchitectureProgram program = ArchitectureProgram());
	
	void GenerateBuilding();

	float GetTotalArea();

	void draw(int floor = -1);
};