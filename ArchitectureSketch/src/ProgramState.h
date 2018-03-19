#pragma once

#include "SharedData.h"
#include "ofxState.h"
#include "ofxGui.h"
#include "ofxEasyCam.h"
#include "Building.h"
#include "Measurements.h"
#include "ArchitectureState.h"

#define ProgramState_StateName "ProgramState"

class ProgramState : public itg::ofxState<SharedData>
{
private:

	bool drawOutlines = true;

	ofLight light;
	ofxEasyCam camera;

	ofRectangle mapRectangle;

	ofxPanel guiPanel;

	ofParameterGroup mProgramParameters;
	ofParameterGroup mSiteParameters;

	ArchitectureProgram* pProgram;

	// program parameters
	ofParameter<int> mInhabitants = ofParameter<int>("Inhabitants", 4, 1, 7);
	ofParameter<int> mStories = ofParameter<int>("Stories", 2, 1, 3);

	// site parameters
	ofParameter<int> mWidth = ofParameter<int>("Lot width (m)", 10, 5, 25);
	ofParameter<int> mDepth = ofParameter<int>("Lot depth (m)", 10, 5, 25);

	ofParameter<bool> mTerracedLeft = ofParameter<bool>("Terraced Left", false);
	ofParameter<bool> mTerracedRight = ofParameter<bool>("Terraced Right", false);

	ofxSlider<int> mInhabitantsSlider = ofxSlider<int>(mInhabitants, 300, 20);
	ofxSlider<int> mStoriesSlider = ofxSlider<int>(mStories, 300, 20);

	ofxSlider<int> mWidthSlider = ofxSlider<int>(mWidth, 300, 20);
	ofxSlider<int> mDepthSlider = ofxSlider<int>(mDepth, 300, 20);

	ofxToggle mTerracedLeftToggle = ofxToggle(mTerracedLeft, 300, 20);
	ofxToggle mTerracedRightToggle = ofxToggle(mTerracedRight, 300, 20);


	ofxButton mAcceptButton = ofxButton();

public:
	ProgramState();

	~ProgramState();

	void stateEnter();
	void stateExit();
	void mousePressed(int x, int y, int button);

	void setup();
	void update();
	void draw();
	void drawSiteLayout();

	string getName();
	void keyPressed(int key);

	void updateProgram();

	void acceptButtonPressed();
};

