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
	static ofMatrix4x4 Convert2DTo3D()
	{
		// matrix that swaps y and z
		return ofMatrix4x4(
			1, 0, 0, 0,
			0, 0, 1, 0,
			0, 1, 0, 0,
			0, 0, 0, 1);
	}
	
	//--------------------------------------------------------------
	static ofPath PolylineToPath(ofPolyline& polyline)
	{
		ofPath path;

		for (int i = 0; i < polyline.getVertices().size(); i++) 
		{
			if (i == 0) {
				path.newSubPath();
				path.moveTo(polyline.getVertices()[i]);
			}
			else {
				path.lineTo(polyline.getVertices()[i]);
			}
		}

		path.close();
		
		return path;
		//path.simplify();
	}

	//--------------------------------------------------------------
	static void AddCap(ofMesh& mesh, ofPolyline outline, ofVec3f offset)
	{
		//vector<ofVec2f> points;

		//for (int i = 0; i < outline.size(); i++)
		//{
		//	points.push_back(outline[i]);
		//}

		ofPath p = PolylineToPath(outline);
		//p.getTessellation();
		ofMesh m = p.getTessellation();
			// ofxDelaunay2D::triangulate(points);

		// 2d to 3d conversion matrix
		ofMatrix4x4 mat2dto3d = Convert2DTo3D();

		// add to mesh
		vector<ofMeshFace> faces = m.getUniqueFaces();

		for (size_t i = 0; i < faces.size(); i++)
		{
			ofVec3f v0 = faces[i].getVertex(0);
			ofVec3f v1 = faces[i].getVertex(1);
			ofVec3f v2 = faces[i].getVertex(2);

			// calculate triangle centerpoint
			ofVec3f center = ( v0 + v1 + v2 ) / 3.0f;
			
			// check whether the triangle center lies inside the polygon, if so add it to the mesh
			if (outline.inside(center))
			{
				v0 = mat2dto3d * v0 + offset;
				v1 = mat2dto3d * v1 + offset;
				v2 = mat2dto3d * v2 + offset;

				AddFace(mesh, v0, v1, v2);
			}
		}

//		mesh.append(m);
	}
};