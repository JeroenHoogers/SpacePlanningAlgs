#include "DebugState.h"

//--------------------------------------------------------------
DebugState::DebugState()
{
}

//--------------------------------------------------------------
DebugState::~DebugState()
{
}

//--------------------------------------------------------------
void DebugState::stateEnter()
{

}

//--------------------------------------------------------------
void DebugState::stateExit()
{

}

//--------------------------------------------------------------
void DebugState::setup()
{
	polygon = ofPolyline();
	polygon.addVertex(ofPoint(20, 20));
	polygon.addVertex(ofPoint(20, 400));
	polygon.addVertex(ofPoint(350, 600));
	polygon.addVertex(ofPoint(400, 700));
	polygon.addVertex(ofPoint(410, 750));
	polygon.addVertex(ofPoint(800, 400));
	polygon.addVertex(ofPoint(600, 20));
	polygon.close();

	rays.push_back(LineSegment(ofPoint(50, 50), ofPoint(500, 500)));
	rays.push_back(LineSegment(ofPoint(200, 50), ofPoint(100, 400)));
}

//--------------------------------------------------------------
void DebugState::update()
{

}

//--------------------------------------------------------------
void DebugState::draw()
{
	//ofClear()
	ofSetLineWidth(2);
	ofBackground(200);

	ofPushMatrix();
	{
		//ofTranslate(20, 20);
		
		ofSetLineWidth(2.0f);
		//for (size_t i = 0; i < rays.size(); i++)
		//{
		//	ofDrawLine(rays[i].v1, rays[i].v2);
		//	ofDrawCircle(rays[i].v1, 3);
		//}

		//ofSetColor(20);
		//ofPoint intersection;
		//if (IntersectionHelper::intersectRays(
		//	rays[0].v1, rays[0].v2 - rays[0].v1, 
		//	rays[1].v1, rays[1].v2 - rays[1].v1, 
		//	&intersection))
		//{
		//	ofDrawCircle(intersection, 4);
		//}

		ofSetColor(20);


		polygon.draw();

		// generate straight skeleton
		vector<LineSegment> arcs;

		SSAlgOutput straightSkeleton = StraightSkeleton::CreateSkeleton(polygon, steps);

		// unpack tuple
		std::tie(arcs, faces) = straightSkeleton;

		skeleton = arcs;

		ofSetLineWidth(2);
		ofSetColor(20, 20, 200);
		ofNoFill();

		for (size_t i = 0; i < skeleton.size(); i++)
		{
			ofLine(skeleton[i].v1.x, skeleton[i].v1.y, skeleton[i].v2.x, skeleton[i].v2.y);
		}

		ofSetColor(200, 40, 40);
		ofFill();
		//for (size_t i = 0; i < faces.size(); i++)
		//{
			faces[3].draw();
		//}

		
		//faces[2].draw();
		//faces[4].draw();
		//faces[0].draw();
		//faces[3].draw();

		ofNoFill();
	}
	ofPopMatrix();
}

//--------------------------------------------------------------
string DebugState::getName()
{
	return DebugState_StateName;
}

//--------------------------------------------------------------
void DebugState::keyPressed(int key)
{
	if (key == '[')
		steps = ofClamp(steps - 1, 0, 500);

	if (key == ']')
		steps = ofClamp(steps + 1, 0, 500);
}

//--------------------------------------------------------------
void DebugState::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void DebugState::mouseDragged(int x, int y, int button)
{
	ofPoint mousePos = ofPoint(x, y);
	// drag vertices
	for (size_t i = 0; i < polygon.size(); i++)
	{
		if (polygon[i].distance(mousePos) < 15.0f)
		{
			polygon[i] = mousePos;
			break;
		}
	}


	//for (size_t i = 0; i < rays.size(); i++)
	//{
	//	if (rays[i].v1.distance(mousePos) < 10.0f)
	//	{
	//		rays[i].v1 = mousePos;
	//		break;
	//	}

	//	if (rays[i].v2.distance(mousePos) < 10.0f)
	//	{
	//		rays[i].v2 = mousePos;
	//		break;
	//	}
	//}
}