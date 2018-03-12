#pragma once

#include "ofMain.h"

class MeshHelper
{
public:
	//--------------------------------------------------------------
	static void AddFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c) 
	{
		ofVec3f normal = ((b - a).cross(c - a)).normalize();
		mesh.addNormal(normal);
		mesh.addVertex(a);
		mesh.addNormal(normal);
		mesh.addVertex(b);
		mesh.addNormal(normal);
		mesh.addVertex(c);
	};

	//--------------------------------------------------------------
	static void AddFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d) 
	{
		AddFace(mesh, a, b, c);
		AddFace(mesh, a, c, d);
	};

	//--------------------------------------------------------------
	static ofVec3f CalculateFaceCenter(ofMeshFace *face) 
	{
		int lastPointIndex = 3;
		ofVec3f result;
		for (unsigned int i = 0; i < 3; i++) {
			result += face->getVertex(i);
		}
		result /= lastPointIndex;
		return result;
	};

	//--------------------------------------------------------------
	static void AddCap(ofMesh& mesh, ofPolyline line, ofVec3f offset)
	{
		
	}
};