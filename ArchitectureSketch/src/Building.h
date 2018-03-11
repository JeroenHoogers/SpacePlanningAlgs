#include "ofMain.h"

#pragma once

struct Subdivision
{
	int faceId;
	float cutPos; // could be an interpolation along the entire shape
};

struct Extrusion
{
	// could be encoded in a single float where the interpolation variable t is derived from the local position on the line segment

	float faceIndex;	// the face to extrude
	float amount;		// the extrusion amount
	float angle;		// angle of extrusion (limited between -45 and + 45 degrees) 
};

class Building
{
private:

	float floorHeight = 3.0f;

	vector<ofPolyline> floorShapes;
	ofMesh buildingMesh;

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

		// create subdivisions
		for (size_t i = 0; i < subdivs.size(); i++)
		{
			
		}

		// do extrusions
		for (size_t i = 0; i < extrusions.size(); i++)
		{

		}

		// create floors
		int floors = 3;
		for (size_t i = 0; i < floors; i++)
		{
			floorShapes.push_back(baseFloor);
		}

		// TODO: per floor subdivisions / extrude
	};

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
				int j2 = (j - 1) % floorShapes[i].size();

				addFace(buildingMesh,
					floorShapes[i][j2] + heightOffset,
					floorShapes[i][j] + heightOffset,
					floorShapes[i][j] + heightOffset + ofVec3f(0, floorHeight),
					floorShapes[i][j2] + heightOffset + ofVec3f(0, floorHeight));


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

	//--------------------------------------------------------------
	void addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c) {
		ofVec3f normal = ((b - a).cross(c - a)).normalize();
		mesh.addNormal(normal);
		mesh.addVertex(a);
		mesh.addNormal(normal);
		mesh.addVertex(b);
		mesh.addNormal(normal);
		mesh.addVertex(c);
	}

	//--------------------------------------------------------------
	void addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d) {
		addFace(mesh, a, b, c);
		addFace(mesh, a, c, d);
	}


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

	void GenerateBuilding()
	{
		// generate the floor shapes
		generateFloorShapes();

		// generate the building mesh using the floor shapes
		generateMesh();
	}


	// get the total building area
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

	// draw building
	void draw()
	{
		//ofSetColor(200);
		buildingMesh.drawFaces();
	};
};


