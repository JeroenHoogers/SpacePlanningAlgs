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
	polygon.addVertex(ofPoint(0, 0));
	polygon.addVertex(ofPoint(20, 400));
	polygon.addVertex(ofPoint(400, 700));
	//polygon.addVertex(ofPoint(800, 400));
	polygon.addVertex(ofPoint(600, 0));
	polygon.close();


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
		ofTranslate(20, 20);
		ofSetColor(20);


		polygon.draw();

		skeleton = StraightSkeleton::CreateSkeleton(polygon, steps);


		ofSetLineWidth(1);
		ofSetColor(20, 20, 200);

		for (size_t i = 0; i < skeleton.size(); i++)
		{
			ofLine(skeleton[i].v1, skeleton[i].v2);
		}
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
	if (key == '-')
		steps = ofClamp(steps - 1, 0, 500);

	if (key == '=')
		steps = ofClamp(steps + 1, 0, 500);
}

//--------------------------------------------------------------
void DebugState::mousePressed(int x, int y, int button)
{

}