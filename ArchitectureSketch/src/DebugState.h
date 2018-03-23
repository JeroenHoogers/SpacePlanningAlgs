#pragma once

#include "SharedData.h"
#include "ofxState.h"
#include "ofxGui.h"
#include "StraightSkeleton.h"

#define DebugState_StateName "DebugState"

class DebugState : public itg::ofxState<SharedData>
{
private:
	vector<SkeletonArc> skeleton;
	ofPolyline polygon;

public:
	DebugState();

	~DebugState();

	void stateEnter();
	void stateExit();
	void mousePressed(int x, int y, int button);

	void setup();
	void update();
	void draw();

	string getName();
	void keyPressed(int key);
};

