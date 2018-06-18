#include "SharedData.h"
#include "ofxState.h"
#include "ofxGui.h"
#include "ofxEasyCam.h"
#include "Building.h"
//#include "ArchitectureState.h"
//#include "ProgramState.h"

#define ProjectState_StateName "ProjectState"

enum struct EProjectStep
{
	Exterior,
	BFSInterior,
	SplitInterior,
};

#pragma once
class ProjectState : public itg::ofxState<SharedData>
{
private:
	bool drawOutlines = true;

	ofLight light;
	ofxEasyCam camera;

	Building building;

	ofParameter<int> mVisibleFloor = -1;

	GeneticAlgorithm<float> geneticAlgorithm;

	ArchitectureProgram* pProgram;

	EProjectStep step = EProjectStep::Exterior;

	ofxButton mInterior1Button;
	ofxButton mInterior2Button;
	ofxButton mBackButton;

	ofxButton mFloorUpButton;
	ofxButton mFloorDownButton;

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

	void gotoNextStep();

	string getName();
	void keyPressed(int key);

	void backButtonPressed();
	void interior1ButtonPressed();
	void interior2ButtonPressed();

	void floorUpButtonPressed();
	void floorDownButtonPressed();
};

