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
	float floorHeight = 4.0f;

	vector<ofPolyline> floorShapes;
	ofMesh buildingMesh;

	vector<ofPolyline> lines;

	ofPolyline parcel;

	void generateFloorShapes()
	{
		// construct polylines for each floor
		floorShapes.clear();

		ofPolyline baseFloor = ofPolyline();

		// create base rectangle
		baseFloor.addVertex(boundingBox.getTopLeft());
		baseFloor.addVertex(boundingBox.getBottomLeft());
		baseFloor.addVertex(boundingBox.getBottomRight());
		baseFloor.addVertex(boundingBox.getTopRight());

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

		// TODO: multi level extrusions?
		for (size_t i = 0; i < floors; i++)
		{
			ofPolyline pl = baseFloor;
			
			// do extrusions for individual floors
			applyExtrusions(pl, i);

			floorShapes.push_back(pl);
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

			float t = extrusions[i].position;

			ofPoint p = floorshape.getPointAtPercent(t);
			int index = ceilf(floorshape.getIndexAtPercent(t));

			// due to mutation clamping this could become zero which is invalid
			// TODO: wrap to last vertex?
			if (index == 0)
				index = 1;

			//ofVec3f n = floorshape.getNormalAtIndexInterpolated(t);

			// extrude
			floorshape.insertVertex(p, index);
			floorshape.insertVertex(p, index);
		
			//baseFloor.;

			// calculate face normal
			ofVec3f diff = floorshape[index - 1] - floorshape[index];
			ofVec3f n = ofVec2f(-diff.y, diff.x);

			// calculate rotated normal
			float angle = extrusions[i].extrudeAngle;

			n.normalize();

			// calculate new points
			ofVec3f u = n * extrusions[i].extrudeAmount;

			ofPoint p1 = floorshape[index] + u;
			ofPoint p2 = floorshape[index - 1] + u;

			if (angle != 0)
			{
				//// center coordinate system along the extusion edge
				//ofMatrix4x4 translation = ofMatrix4x4::newTranslationMatrix((floorshape[index] + floorshape[index - 1]) * 0.5f);
				//ofMatrix4x4 rotation = ofMatrix4x4::newRotationMatrix(angle, ofVec3f(0, 0, 1));

				//// do rotation
				//p1 = //translation.getInverse() * rotation * translation * p1;
				//p2 = translation.getInverse() * rotation * translation * p2;


				// project p onto u
				if (angle > 0)
					p1 = floorshape[index];
				else
					p2 = floorshape[index - 1];
			}

			// do extrusion
			floorshape[index] = p1;
			floorshape[index - 1] = p2;
		}
	}

	//--------------------------------------------------------------
	void generateMesh()
	{
		// empty mesh
		buildingMesh.clear();

		lines.clear();

		int verts = 0;

		// 2d to 3d matrix
		ofMatrix4x4 mat = MeshHelper::Convert2DTo3D();

		// add new vertices / triangles
		for (size_t i = 0; i < floorShapes.size(); i++)
		{
			// calculate current floor height
			float currentHeight = floorHeight * i;
			ofVec3f bottomHeightOffset = ofVec3f(0, currentHeight);
			ofVec3f topHeightOffset = ofVec3f(0, currentHeight + floorHeight);

			ofPolyline floorOutline;
			ofPolyline ceilingOutline;

			// add floor vertices
			for (size_t j = 0; j < floorShapes[i].size(); j++)
			{
				int j2 = (j + 1) % floorShapes[i].size();

				// add wall face
				MeshHelper::AddFace(buildingMesh,
					mat * floorShapes[i][j] + bottomHeightOffset,
					mat * floorShapes[i][j2] + bottomHeightOffset,
					mat * floorShapes[i][j2] + topHeightOffset,
					mat * floorShapes[i][j] + topHeightOffset);

				// add vertex to floor outline
				floorOutline.addVertex(mat * floorShapes[i][j] + bottomHeightOffset);
				ceilingOutline.addVertex(mat * floorShapes[i][j] + topHeightOffset);

				// create vertical edges
				ofPolyline wallEdge;
				wallEdge.addVertex(mat * floorShapes[i][j] + bottomHeightOffset);
				wallEdge.addVertex(mat * floorShapes[i][j] + topHeightOffset);

				lines.push_back(wallEdge);
			} 

			floorOutline.close();
			ceilingOutline.close();

			lines.push_back(floorOutline);
			lines.push_back(ceilingOutline);

			// add flat roof
			//if (i == floorShapes.size() - 1)
			//	MeshHelper::AddCap(buildingMesh, floorShapes[i], topHeightOffset);

			// create cap between floors (only where the floor differs)
			MeshHelper::AddCap(buildingMesh, floorShapes[i], topHeightOffset);

			//if (i == 0)
			MeshHelper::AddCap(buildingMesh, floorShapes[i], bottomHeightOffset);

			// create faces automatically
			buildingMesh.setupIndicesAuto();
		}
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
	void LoadFromGenotype(Genotype gt)
	{
		//GeneticAlgorithm ga = GeneticAlgorithm();
		//ga.setup(1, 12);

		//Genotype gt = ga.generateRandomDna();

		// first 2 define params define bounding volume
		float w = 13.0f + floorf(gt[0] * 9.0f) * 2;
		float h = 13.0f + floorf(gt[1] * 9.0f) * 2;

		int maxFloors = 3;

		boundingBox = ofRectangle(
			-w * 0.5f, -h * 0.5f, w, h);

		// TODO: derive nr of floors from area
		floors = fminf(floorf(gt[2] * maxFloors + 1.0f), maxFloors);
		
		// TODO: separate subdivs and extrusions

		//subdivs.clear();
		extrusions.clear();

		float minExtrusion = 1.0f;
		float maxExtrusion = 6.0f;

		for (size_t i = 3; i < gt.size()-3; i+=4)
		{
			// which floor does this extrusion apply to?
			// TODO: tweak this ratio
			
			// TODO: make sure this is not close to any walls, maybe align to a grid?
			float position = gt[i]; // interpolation along shape
			// TODO: apply to multiple but not all floors?
			int floor = (gt[i + 1] > 0.5f) ? fminf(floorf((gt[i + 1] - 0.5f) * 2.0f * floors), floors-1) : -1;

			float amount = ofLerp(-maxExtrusion, maxExtrusion, gt[i + 2]);
			amount = (amount < 0) ? ofClamp(amount, -maxExtrusion, -minExtrusion) : ofClamp(amount, minExtrusion, maxExtrusion);

			// calculate angle
			float angle = 0;
			if (gt[i + 3] < 0.25f)
				angle = ofLerp(-45.0f, 0, gt[i + 3] * 4);
			if (gt[i + 3] > 0.75f)
				angle = ofLerp(0, 45.0f, (gt[i + 3] - 0.75f) * 4);

			// create subdivs
			//subdivs.push_back(Subdivision(gt[i]));
			
			// add extrusion
			extrusions.push_back(
				Extrusion(position, amount, floor, angle));
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
			area += fabs(floorShapes[i].getArea());
		}

		// TODO: check if this is correct
		area *= 0.1f;

		// round area to 1 decimal
		area = roundf(area * 100) / 100;

		return area;
	};

	//--------------------------------------------------------------
	void draw()
	{
		// HACK: hack depth buffer range to make sure the lines render on top of the geometry
		glDepthRange(0.0005, 1.0);
		//ofSetColor(200);

		// draw building
		ofSetColor(255);
		buildingMesh.drawFaces();

		// HACK: hack depth buffer range to make sure the lines render on top of the geometry
		glDepthRange(0.0, 0.9995);

		ofSetColor(50);
		
		// draw lines
		for (size_t i = 0; i < lines.size(); i++)
		{
			// draw floor outlines slightly thicker then the vertical edges
			if(lines[i].size() > 2)
				ofSetLineWidth(1.0f);
			else
				ofSetLineWidth(0.7f);
			lines[i].draw();
		}

		// restore depth buffer
		glDepthRange(0.0, 1.0);

		// draw wireframe
		//buildingMesh.drawWireframe();

		// DEBUG: draw normals
		//MeshHelper::drawNormals(buildingMesh);
	};
};