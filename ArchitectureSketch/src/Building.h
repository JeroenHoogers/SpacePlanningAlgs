#pragma once

#include "ofMain.h"
#include "GeneticAlgorithm.h"
#include "MeshHelper.h" 

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

	Extrusion(float pos, float amount, int floor)
	{
		faceIndex = pos;
		extrudeAmount = amount;
		extrudeFloor = floor;
	}

	float faceIndex;		// the face to extrude
	float extrudeAmount;	// the extrusion amount
	int extrudeFloor = -1;	// apply the extrusion to a specific floor (-1) applies it to all floors

	float angle;			// angle of extrusion (limited between -45 and + 45 degrees) 
};

class Building
{
private:

	int floors = 3;
	float floorHeight = 3.0f;

	vector<ofPolyline> floorShapes;
	ofMesh buildingMesh;

	ofPolyline parcel;

	void generateFloorShapes()
	{
		// construct polylines for each floor
		floorShapes.clear();

		ofPolyline baseFloor = ofPolyline();

		// create square
		baseFloor.addVertex(
			ofPoint(boundingBox.getMinX(), 0, boundingBox.getMinY()));
		baseFloor.addVertex(
			ofPoint(boundingBox.getMinX(), 0, boundingBox.getMaxY()));
		baseFloor.addVertex(
			ofPoint(boundingBox.getMaxX(), 0, boundingBox.getMaxY()));
		baseFloor.addVertex(
			ofPoint(boundingBox.getMaxX(), 0, boundingBox.getMinY()));

		baseFloor.close();
		//baseFloor.addVertex(
		//	ofPoint(boundingBox.getMinX(), 0, boundingBox.getMinY()));

		//// create subdivisions
		//for (size_t i = 0; i < subdivs.size(); i++)
		//{
		//	// TODO: add the same vertex twice
		//	float t = subdivs[i].cutpos;
		//	ofPoint p = baseFloor.getPointAtPercent(t);

		//	int index = ceilf(baseFloor.getIndexAtPercent(t));
		//	//int i2 = ceilf(baseFloor.getIndexAtPercent(t));

		//	// insert vertex twice
		//	baseFloor.insertVertex(p, index);
		//	baseFloor.insertVertex(p, index);
		//}

		// do base floor extrusions
		applyExtrusions(baseFloor, -1);

		// create floors
		//floors = 3;
		for (size_t i = 0; i < floors; i++)
		{
			//ofPolyline pl = baseFloor;
			
			// do extrusions for individual floors
			applyExtrusions(baseFloor, i);

			floorShapes.push_back(baseFloor);

		}

		// TODO: per floor subdivisions / extrude
	};

	//--------------------------------------------------------------
	void applyExtrusions(ofPolyline &floorshape, int floor)
	{
		// do extrusions
		for (size_t i = 0; i < extrusions.size(); i++)
		{
			if (extrusions[i].extrudeFloor != floor)
				continue;

			float t = extrusions[i].faceIndex;

			ofPoint p = floorshape.getPointAtPercent(t);
			int index = ceilf(floorshape.getIndexAtPercent(t));

			// extrude
			floorshape.insertVertex(p, index);
			floorshape.insertVertex(p, index);
			//baseFloor.;

			// calculate face snormal
			ofVec3f diff = floorshape[index - 1] - floorshape[index];
			ofVec3f n = ofVec3f(-diff.z, 0, diff.x);

			n.normalize();

			// extrude inward
			floorshape[index] += n * extrusions[i].extrudeAmount;
			floorshape[index - 1] += n * extrusions[i].extrudeAmount;
		}
	}

	//--------------------------------------------------------------
	void generateMesh()
	{
		// empty mesh
		buildingMesh.clear();

		int verts = 0;

		// add new vertices / triangles
		for (size_t i = 0; i < floorShapes.size(); i++)
		{
			// calculate current floor height
			float currentHeight = floorHeight * i;
			ofVec3f heightOffset = ofVec3f(0, currentHeight);

			// add floor vertices
			for (size_t j = 0; j < floorShapes[i].size(); j++)
			{
				int j2 = (j + 1) % floorShapes[i].size();

				MeshHelper::AddFace(buildingMesh,
					floorShapes[i][j] + heightOffset,
					floorShapes[i][j2] + heightOffset,
					floorShapes[i][j2] + heightOffset + ofVec3f(0, floorHeight),
					floorShapes[i][j] + heightOffset + ofVec3f(0, floorHeight));

				//// add vertices
				//buildingMesh.addVertex(floorShapes[i][v] + heightOffset);
				//buildingMesh.addVertex(floorShapes[i][v] + heightOffset + ofVec3f(0, floorHeight));

				//// add normals;
				////bu

				//if (j > 0)
				//{
				//	int j2 = verts;
				//	buildingMesh.addTriangle(j2 - 2 , j2 - 1, j2);
				//	buildingMesh.addTriangle(j2 - 1, j2, j2 + 1);
				//}

				//verts += 2;
			}

			// create cap between floors (only where the floor differs)
			if (i > 0)
			{
				//for (size_t j = 0; j < floorShapes[i-1].size(); j++)
				//{
				//	buildingMesh.addVertex(floorShapes[i-1][j]);
				//	buildingMesh.addVertex(floorShapes[i-1][j] + ofVec3f(0, floorHeight));
				//}
			}
		}
	
		//buildingMesh.nor
		//buildingMesh.a
	};




public:
	ofRectangle boundingBox = ofRectangle(-10, -20, 20, 30);
	vector<Subdivision> subdivs;
	vector<Extrusion> extrusions;

	Building()
	{
		buildingMesh = ofMesh();
		subdivs = vector<Subdivision>();
		extrusions = vector<Extrusion>();
	}

	~Building()
	{

	}

	//--------------------------------------------------------------
	void LoadFromGenotype()
	{
		GeneticAlgorithm ga = GeneticAlgorithm();
		ga.setup(1, 12);

		Genotype gt = ga.generateRandomDna();

		// first 2 define params define bounding volume
		float w = 10 + floorf(gt[0] * 10.0f) * 2;
		float h = 10 + floorf(gt[1] * 10.0f) * 2;

		int maxFloors = 3;

		boundingBox = ofRectangle(
			-w * 0.5f, -h * 0.5f, w, h);

		// TODO: derive nr of floors from area
		floors = fminf(floorf(gt[2] * maxFloors + 1.0f), maxFloors);
		
		// TODO: separate subdivs and extrusions

		//subdivs.clear();
		extrusions.clear();

		float minExtrusion = 2.0f;
		float maxExtrusion = 6.0f;

		for (size_t i = 3; i < gt.size(); i+=3)
		{
			// which floor does this extrusion apply to?
			int floor = (gt[i + 2] > 0.5f) ? fminf(floorf((gt[i + 1] - 0.5f) * 2.0f * floors), floors-1) : -1;

			// create subdivs
			//subdivs.push_back(Subdivision(gt[i]));
			extrusions.push_back(
				Extrusion(
					gt[i],
					minExtrusion + floorf(gt[i + 1] * maxExtrusion - minExtrusion),
					floor));
		}

		// create parcel
		parcel.clear();

		parcel.addVertex(
			ofPoint(boundingBox.getMinX(), 0, boundingBox.getMinY()));
		parcel.addVertex(
			ofPoint(boundingBox.getMinX(), 0, boundingBox.getMaxY()));
		parcel.addVertex(
			ofPoint(boundingBox.getMaxX(), 0, boundingBox.getMaxY()));
		parcel.addVertex(
			ofPoint(boundingBox.getMaxX(), 0, boundingBox.getMinY()));
		parcel.close();
	}

	//--------------------------------------------------------------
	void GenerateBuilding()
	{
		// generate the floor shapes
		generateFloorShapes();

		// generate the building mesh using the floor shapes
		generateMesh();
	}


	//--------------------------------------------------------------
	float GetTotalArea()
	{
		float area = 0;

		// approximation
		for (size_t i = 0; i < floorShapes.size(); i++)
		{
			area += floorShapes[i].getArea();
		}

		return area;
	};

	//--------------------------------------------------------------
	void draw()
	{
		//ofSetColor(200);

		// draw parcel
		ofSetColor(10);
		parcel.draw();

		ofSetColor(255, 0, 0);
		ofDrawLine(parcel[0], parcel[0] + ofVec3f(0, 1, 0));

		// draw building
		ofSetColor(255);
		buildingMesh.drawFaces();

		ofSetColor(30);
		glPointSize(6);

		// draw vertices
		buildingMesh.drawVertices();

		//ofSetColor(0, 0, 255);

		//buildingMesh.re
	};
};


