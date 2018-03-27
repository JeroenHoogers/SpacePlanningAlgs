#pragma once

#include "SharedData.h"
#include "ofxState.h"
#include "ofxGui.h"
#include "StraightSkeleton.h"

#define DebugState_StateName "DebugState"

class DebugState : public itg::ofxState<SharedData>
{
private:
	vector<LineSegment> skeleton;
	vector<ofPolyline> faces;

	vector<LineSegment> rays;

	ofPolyline polygon;
	int steps = 0;

public:
	DebugState();

	~DebugState();

	void stateEnter();
	void stateExit();
	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);

	void setup();
	void update();
	void draw();

	string getName();
	void keyPressed(int key);
};

