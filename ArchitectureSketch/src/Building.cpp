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


	for (size_t i = 0; i < mFloors; i++)
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
	for (size_t i = 0; i < mExtrusions.size(); i++)
	{
		if (mExtrusions[i].extrudeFloor != floor)
			continue;

		float t = mExtrusions[i].position;

		ofPoint p = floorshape.getPointAtPercent(t);

		int index = ceilf(floorshape.getIndexAtPercent(t));
		if (index == 0)
			index = 1;
		//if (mExtrusions[i].extrudeSide)
		//	index = floorf(floorshape.getIndexAtPercent(t));

		// fixes crash that occurs when the vertex is invalid
		//index = floorshape.getWrappedIndex(index);
		int im1 = floorshape.getWrappedIndex(index-1);

		// due to mutation clamping this could become zero which is invalid

		//ofVec3f n = floorshape.getNormalAtIndexInterpolated(t);

		// extrude
		floorshape.insertVertex(p, index);
		floorshape.insertVertex(p, index);

		int i1 = floorshape.getWrappedIndex(index);
		int i2 = floorshape.getWrappedIndex(index - 1);

		if (mExtrusions[i].extrudeSide)
		{
			i1 = floorshape.getWrappedIndex(index + 1);
			i2 = floorshape.getWrappedIndex(index + 2);
		}

		// calculate face normal
		ofVec3f diff = floorshape[im1] - floorshape[index];
		ofVec3f n = ofVec2f(-diff.y, diff.x);

		// calculate rotated normal
		//float angle = extrusions[i].extrudeAngle;

		float order = mExtrusions[i].ordering;

		n.normalize();

		// calculate new points
		ofVec3f u = n * mExtrusions[i].extrudeAmount;

		ofPoint p1 = floorshape[i1] + u;
		ofPoint p2 = floorshape[i2] + u;

		//if (mExtrusions[i].extrudeSide)
		//	p2 = floorshape[index + 1] + u;

		//if (angle != 0)
		//{
		//	//// center coordinate system along the extusion edge
		//	//ofMatrix4x4 translation = ofMatrix4x4::newTranslationMatrix((floorshape[index] + floorshape[index - 1]) * 0.5f);
		//	//ofMatrix4x4 rotation = ofMatrix4x4::newRotationMatrix(angle, ofVec3f(0, 0, 1));

		//	//// do rotation
		//	//p1 = //translation.getInverse() * rotation * translation * p1;
		//	//p2 = translation.getInverse() * rotation * translation * p2;


		//	// project p onto u
		//	if (angle > 0)
		//		p1 = floorshape[index];
		//	else
		//		p2 = floorshape[index - 1];
		//}

		// do extrusion
		floorshape[i1] = p1;
		floorshape[i2] = p2;
	}

	// simplify shape
	floorshape.simplify(0.001f);
}

//--------------------------------------------------------------
void Building::Create(float width, float height, int floors, vector<Extrusion> extrusions, ERoofType roof, float roofPitch)
{
	// Create building
	mFloors = floors;

	mRoofType = roof;
	mRoofPitch = roofPitch;

	boundingBox = ofRectangle(-width * 0.5f, -height * 0.5f, width, height);
	mExtrusions = extrusions;

	parcel.clear();
	parcel.addVertex(ofPoint(boundingBox.getMinX(), 0, boundingBox.getMinY()));
	parcel.addVertex(ofPoint(boundingBox.getMinX(), 0, boundingBox.getMaxY()));
	parcel.addVertex(ofPoint(boundingBox.getMaxX(), 0, boundingBox.getMaxY()));
	parcel.addVertex(ofPoint(boundingBox.getMaxX(), 0, boundingBox.getMinY()));
	parcel.close();

	// generate floor shapes
	generateFloorShapes();
}
//
////--------------------------------------------------------------
//void Building::LoadFromGenotype(vector<float> gt, ArchitectureProgram program)
//{
//	// TODO: loosly couple building and program
//	// TODO: move this to exterior evolver
//
//	//GeneticAlgorithm ga = GeneticAlgorithm();
//	//ga.setup(1, 12);
//
//	//Genotype gt = ga.generateRandomDna();
//
//	// first 2 define params define bounding volume
//	// TODO: Fix scaling
//	float w = 5.0f + floorf(gt[0] * (program.lotWidth - 5.0f));
//	float h = 5.0f + floorf(gt[1] * (program.lotDepth - 5.0f));
//
//	if (program.terracedLeft && program.terracedRight)
//		w = program.lotWidth;
//
//	int maxFloors = 3;
//
//	boundingBox = ofRectangle(
//		-w * 0.5f, -h * 0.5f, w, h);
//
//	// TODO: derive nr of floors from area
//	//floors = fminf(floorf(gt[2] * maxFloors + 1.0f), maxFloors);
//	int roofSelector = floorf(gt[2] * 2.0f);
//
//	if (roofSelector == 0)
//		mRoofType = ERoofType::Flat;
//	else
//		mRoofType = ERoofType::Hip;
//
//	mRoofPitch = gt[2] / 2.0f; // roof param
//	
//	
//	mFloors = program.stories;
//	// TODO: separate subdivs and extrusions
//
//	//subdivs.clear();
//	mExtrusions.clear();
//
//	float minExtrusion = 1.0f;
//	float maxExtrusion = 1.0f + 2.5f;
//
//	for (size_t i = 3; i < gt.size() - 3; i += 4)
//	{
//		// which floor does this extrusion apply to?
//		// TODO: tweak this ratio
//
//		// TODO: make sure this is not close to any walls, maybe align to a grid?
//		float position = gt[i]; // interpolation along shape
//								// TODO: apply to multiple but not all floors?
//		int floor = (gt[i + 1] > 0.5f) ? fminf(floorf((gt[i + 1] - 0.5f) * 2.0f * mFloors), mFloors - 1) : -1;
//
//		float amount = ofLerp(-maxExtrusion, maxExtrusion, gt[i + 2]);
//		amount = (amount < 0) ? ofClamp(amount, -maxExtrusion, -minExtrusion) : ofClamp(amount, minExtrusion, maxExtrusion);
//
//		// calculate angle
//		float angle = 0;
//		//if (gt[i + 3] < 0.1f)
//		//	angle = ofLerp(-45.0f, 0, gt[i + 3] * 4);
//		//if (gt[i + 3] > 0.9f)
//		//	angle = ofLerp(0, 45.0f, (gt[i + 3] - 0.75f) * 4);
//
//		// create subdivs
//		//subdivs.push_back(Subdivision(gt[i]));
//
//		// add extrusion
//		mExtrusions.push_back(
//			Extrusion(position, amount, floor, angle));
//	}
//
//	// create parcel
//	parcel.clear();
//
//	parcel.addVertex(ofPoint(boundingBox.getMinX(), 0, boundingBox.getMinY()));
//	parcel.addVertex(ofPoint(boundingBox.getMinX(), 0, boundingBox.getMaxY()));
//	parcel.addVertex(ofPoint(boundingBox.getMaxX(), 0, boundingBox.getMaxY()));
//	parcel.addVertex(ofPoint(boundingBox.getMaxX(), 0, boundingBox.getMinY()));
//	parcel.close();
//
//	// generate floor shapes
//	generateFloorShapes();
//}


//--------------------------------------------------------------
void Building::extrudeShape(ofMesh& mesh, ofPolyline shape, ofVec3f bottomOffset, ofVec3f topOffset, bool ccw)
{
	// TODO: fix winding

	// 2d to 3d matrix
	ofMatrix4x4 mat = MeshHelper::Convert2DTo3D();

	ofPolyline bottomOutline;
	ofPolyline topOutline;


	for (size_t j = 0; j < shape.size(); j++)
	{
		int j2 = shape.getWrappedIndex(j + 1);
		//int j2 = (j + 1) % shape.size();

		// add wall face
		if (ccw)
		{
			MeshHelper::AddFace(mesh,
				mat * shape[j] + bottomOffset,
				mat * shape[j2] + bottomOffset,
				mat * shape[j2] + topOffset,
				mat * shape[j] + topOffset);
		}
		else
		{
			MeshHelper::AddFace(mesh,
				mat * shape[j2] + bottomOffset,
				mat * shape[j] + bottomOffset,
				mat * shape[j] + topOffset,
				mat * shape[j2] + topOffset);
		}

		bottomOutline.addVertex(mat * shape[j] + bottomOffset);
		topOutline.addVertex(mat * shape[j] + topOffset);

		//ofPolyline wallEdge1;
		//wallEdge1.addVertex(mat * shape[j] + bottomOffset);
		//wallEdge1.addVertex(mat * shape[j2] + bottomOffset);

		//interiorLines.push_back(wallEdge1);

		//ofPolyline wallEdge2;
		//wallEdge2.addVertex(mat * shape[j] + topOffset);
		//wallEdge2.addVertex(mat * shape[j2] + topOffset);

		//interiorLines.push_back(wallEdge2);


		// add vertex to floor outline
		//floorOutline.addVertex(mat * shape[j] + bottomOffset);
		//ceilingOutline.addVertex(mat * shape[j] + topOffset);

		// create vertical edges
		ofPolyline wallEdge;
		wallEdge.addVertex(mat * shape[j] + bottomOffset);
		wallEdge.addVertex(mat * shape[j] + topOffset);

		interiorLines.push_back(wallEdge);
	}

	interiorLines.push_back(bottomOutline);
	interiorLines.push_back(topOutline);
}

//--------------------------------------------------------------
void Building::generateMesh()
{
	// empty mesh
	buildingMesh.clear();

	floorMeshes.clear();

	lines.clear();

	int verts = 0;

	// 2d to 3d matrix
	ofMatrix4x4 mat = MeshHelper::Convert2DTo3D();

	// add new vertices / triangles
	for (size_t i = 0; i < floorShapes.size(); i++)
	{
		ofMesh floorMesh = ofMesh();

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

			// add to floor mesh
			MeshHelper::AddFace(floorMesh,
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

		MeshHelper::AddCap(floorMesh, floorShapes[i], bottomHeightOffset);

		// create faces automatically
		floorMesh.setupIndicesAuto();

		floorMeshes.push_back(floorMesh);
	}

	// create faces automatically
	buildingMesh.setupIndicesAuto();
}

//--------------------------------------------------------------
void Building::generateInteriorMesh(vector<InteriorRoom> rooms)
{
//	ofMesh interiorMesh;
	interiorMesh.clear();

	// do only first floor
	ofVec3f bottomHeightOffset = ofVec3f(0, 0);
	ofVec3f topHeightOffset = ofVec3f(0, floorHeight);

	for (int i = 0; i < rooms.size(); i++)
	{
		extrudeShape(interiorMesh, rooms[i].shape, bottomHeightOffset, topHeightOffset, false);
	}

	interiorMesh.setupIndicesAuto();
}

//--------------------------------------------------------------
void Building::generateRoof()
{
	roofMesh.clear();

	// only create a mesh for sloped roofs
	if (mRoofType != ERoofType::Hip)
		return;

	//int floors = floorShapes.size();

	if (floorShapes.size() <= 0)
		return;

	// 2d to 3d matrix
	ofMatrix4x4 mat = MeshHelper::Convert2DTo3D();

	ofMatrix4x4 pitch = ofMatrix4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 0.25f + mRoofPitch, 0,
		0, 0, 0, 1);

	// generate straight skeleton
	vector<LineSegment> arcs;
	vector<ofPolyline> faces;

	SSAlgOutput straightSkeleton = StraightSkeleton::CreateSkeleton(floorShapes[mFloors - 1], 200);

	// unpack tuple
	std::tie(arcs, faces) = straightSkeleton;
	
	float height = floorHeight * mFloors;
	
	for (size_t i = 0; i < arcs.size(); i++)
	{
		// TODO: give height
		// TODO: generate planes
		ofPolyline roofEdge;
		roofEdge.addVertex(mat * pitch * arcs[i].v1 + ofPoint(0, height));
		roofEdge.addVertex(mat * pitch * arcs[i].v2 + ofPoint(0, height));

		lines.push_back(roofEdge);
	}

	// multiply roof faces with pitch matrix
	for (size_t i = 0; i < faces.size(); i++)
	{
		for (size_t j = 0; j < faces[i].size(); j++)
		{
			faces[i][j] = pitch * faces[i][j];
		}
	}

	// add cap
	for (size_t i = 0; i < faces.size(); i++)
	{
		MeshHelper::AddCap(roofMesh, faces[i], ofPoint(0, height));
	}

	roofMesh.setupIndicesAuto();
}

//--------------------------------------------------------------
void Building::GenerateBuilding()
{
	// generate the floor shapes
	//generateFloorShapes();

	// generate the building mesh using the floor shapes
	generateMesh();

	// generate roof
	generateRoof();
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
void Building::SetInterior(vector<InteriorRoom> interior)
{
	interiorLines.clear();
	generateInteriorMesh(interior);

	hasInterior = true;
}

//--------------------------------------------------------------
void Building::draw(int floor)
{
	// HACK: hack depth buffer range to make sure the lines render on top of the geometry
	glDepthRange(0.0005, 1.0);
	//ofSetColor(200);

	// draw building
	if (floor == -1)
	{
		ofSetColor(255);

		buildingMesh.drawFaces();
		
		if (mRoofType == ERoofType::Hip)
			roofMesh.drawFaces();
	}
	else
	{
		if (interiorMesh.hasVertices() && floor == 0)
		{
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
			ofSetColor(255);
			floorMeshes[floor].drawFaces();
			glFrontFace(GL_CW);
			glDisable(GL_CULL_FACE);
		}
		else
		{
			ofSetColor(255);
			floorMeshes[floor].drawFaces();
		}
	}

	if (floor == 0)
	{
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		ofSetColor(255);
		interiorMesh.drawFaces();
		glFrontFace(GL_CW);
		glDisable(GL_CULL_FACE);
	}

	// HACK: hack depth buffer range to make sure the lines render on top of the geometry
	glDepthRange(0.0, 0.9995);

	ofSetColor(60);

	// draw lines
	for (size_t i = 0; i < lines.size(); i++)
	{
		// draw floor outlines slightly thicker then the vertical edges
		ofSetLineWidth(0.7f);
		if (lines[i].size() > 2)
			ofSetLineWidth(1.0f);
			
		lines[i].draw();
	}

	// draw lines
	if (floor == 0)
	{
		for (size_t i = 0; i < interiorLines.size(); i++)
		{
			// draw floor outlines slightly thicker then the vertical edges
			ofSetLineWidth(0.7f);
			if (interiorLines[i].size() > 2)
				ofSetLineWidth(1.0f);

			interiorLines[i].draw();
		}
	}

	// restore depth buffer
	glDepthRange(0.0, 1.0);


	// draw wireframe
	//buildingMesh.drawWireframe();

	// DEBUG: draw normals
	MeshHelper::drawNormals(buildingMesh);
	MeshHelper::drawNormals(roofMesh);
};

