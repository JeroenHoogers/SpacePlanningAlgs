#pragma once

#include "SharedData.h"
#include "ofxState.h"
#include "ofxGui.h"
#include "BFSInteriorEvolver.h"
#include "MeshHelper.h"

#define EditState_StateName "EditState"

class EditState : public itg::ofxState<SharedData>
{
private:
	ArchitectureProgram * pProgram;
	BFSInteriorEvolver interiorEvolver;
	vector<ofPoint> centers;
	ofPolyline polygon;

	vector<bool> walls;

	FloorGrid* pFloor;

	int steps = 0;

public:
	EditState();

	~EditState();

	void stateEnter();
	void stateExit();
	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

	void setup();
	void update();
	void draw();

	ofPoint screenToFloor(ofPoint screenPos);
	ofPoint floortoSceen(ofPoint floorPos);

	string getName();
	void keyPressed(int key);
};

