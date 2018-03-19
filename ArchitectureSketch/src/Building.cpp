#include "Building.h"

//--------------------------------------------------------------
void Building::generateFloorShapes()
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

	// do base floor extrusions
	applyExtrusions(baseFloor, -1);


	// TODO: multi level extrusions?
	for (size_t i = 0; i < floors; i++)
	{
		ofPolyline pl = baseFloor;

		// do extrusions for individual floors
		applyExtrusions(pl, i);

		floorShapes.push_back(pl);
	}

	// TODO: per floor subdivisions / extrude
}

//--------------------------------------------------------------
void Building::applyExtrusions(ofPolyline &floorshape, int floor)
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
void Building::LoadFromGenotype(Genotype gt)
{
	//GeneticAlgorithm ga = GeneticAlgorithm();
	//ga.setup(1, 12);

	//Genotype gt = ga.generateRandomDna();

	// first 2 define params define bounding volume
	// TODO: Fix scaling

	float w = 5.0f + floorf(gt[0] * 12.0f);
	float h = 5.0f + floorf(gt[1] * 12.0f);

	int maxFloors = 3;

	boundingBox = ofRectangle(
		-w * 0.5f, -h * 0.5f, w, h);

	// TODO: derive nr of floors from area
	floors = fminf(floorf(gt[2] * maxFloors + 1.0f), maxFloors);

	// TODO: separate subdivs and extrusions

	//subdivs.clear();
	extrusions.clear();

	float minExtrusion = 0.5f;
	float maxExtrusion = 3.0f;

	for (size_t i = 3; i < gt.size() - 3; i += 4)
	{
		// which floor does this extrusion apply to?
		// TODO: tweak this ratio

		// TODO: make sure this is not close to any walls, maybe align to a grid?
		float position = gt[i]; // interpolation along shape
								// TODO: apply to multiple but not all floors?
		int floor = (gt[i + 1] > 0.5f) ? fminf(floorf((gt[i + 1] - 0.5f) * 2.0f * floors), floors - 1) : -1;

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

	// generate floor shapes
	generateFloorShapes();
}

//--------------------------------------------------------------
void Building::generateMesh()
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
}

//--------------------------------------------------------------
void Building::GenerateBuilding()
{
	// generate the floor shapes
	//generateFloorShapes();

	// generate the building mesh using the floor shapes
	generateMesh();
}

//--------------------------------------------------------------
float Building::GetTotalArea()
{
	float area = 0;

	// approximation
	for (size_t i = 0; i < floorShapes.size(); i++)
	{
		area += fabs(floorShapes[i].getArea());
	}

	// TODO: check if this is correct
//	area *= 0.2f;

	// round area to 1 decimal
	area = roundf(area * 100) / 100;

	return area;
};

//--------------------------------------------------------------
void Building::draw()
{
	// HACK: hack depth buffer range to make sure the lines render on top of the geometry
	glDepthRange(0.0005, 1.0);
	//ofSetColor(200);

	// draw building
	ofSetColor(255);
	buildingMesh.drawFaces();

	// HACK: hack depth buffer range to make sure the lines render on top of the geometry
	glDepthRange(0.0, 0.9995);

	ofSetColor(60);

	// draw lines
	for (size_t i = 0; i < lines.size(); i++)
	{
		// draw floor outlines slightly thicker then the vertical edges
		if (lines[i].size() > 2)
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
	MeshHelper::drawNormals(buildingMesh);
};