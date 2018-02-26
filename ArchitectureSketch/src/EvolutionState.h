#include "SharedData.h"
#include "ofxState.h"
#include "ofxGui.h"
#include "GeneticAlgorithm.h"

#define EvolutionState_StateName "EvolutionState"

struct SelectionRect
{
	int index = -1;
	ofRectangle rect = ofRectangle();
	bool mouseover = false;
	bool selected = false;
};

#pragma once
class EvolutionState : public itg::ofxState<SharedData>
{
private:
	int mParamSize = 5;
	//float mMutationRange = 0.1;
	vector<float> mTargets;	
	ofxPanel gui;

	GeneticAlgorithm geneticAlg;

	ofParameter<int> mDimensions = ofParameter<int>("Nr of Params", 5, 1, 20);
	ofParameter<float> mMutationRange = ofParameter<float>("Mutation amount", 0.1, 0.01, 0.5);
	ofParameter<float> mMutationProbability = ofParameter<float>("Mutation deviation", 0.1, 0.01, 0.5);

	vector<SelectionRect> mSelectionRectangles;

	ofxButton generateNextGenButton;

public:
	EvolutionState();
	~EvolutionState();

	void stateEnter();
	void stateExit();
	void mousePressed(int x, int y, int button);
	void mouseMoved(int x, int y);

	void setup();
	void update();
	void draw();

	void setupEvolution();

	void generateButtonPressed();

	void drawGenotype(vector<float> values);

	string getName();
	void keyPressed(int key);

};

