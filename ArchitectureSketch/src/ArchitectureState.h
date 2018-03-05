#include "SharedData.h"
#include "ofxState.h"
#include "ofxGui.h"
#include "ofxEasyCam.h"
#include "ofFbo.h"
#include "ofxPostProcessing.h"

#define ArchitectureState_StateName "ArchitectureState"

#pragma once
class ArchitectureState : public itg::ofxState<SharedData>
{
private:

	ofLight light;
	ofxEasyCam camera;

	int tilesHorizontal = 3;
	int tilesVertical = 3;

public:
	ArchitectureState();
	~ArchitectureState();

	void stateEnter();
	void stateExit();
	void mousePressed(int x, int y, int button);

	void setup();
	void update();
	void draw();
	void drawGUI();
	void drawTile(ofRectangle viewport);

	string getName();
	void keyPressed(int key);

};

