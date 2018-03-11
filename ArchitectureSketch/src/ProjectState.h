#include "SharedData.h"
#include "ofxState.h"
#include "ofxGui.h"
#include "ofxEasyCam.h"
#include "Building.h"

#define ProjectState_StateName "ProjectState"

#pragma once
class ProjectState : public itg::ofxState<SharedData>
{
private:

	bool drawOutlines = true;

	ofLight light;
	ofxEasyCam camera;

	Building building;

public:
	ProjectState();

	~ProjectState();

	void stateEnter();
	void stateExit();
	void mousePressed(int x, int y, int button);

	void setup();
	void update();
	void draw();
	void drawGUI();

	string getName();
	void keyPressed(int key);

};

