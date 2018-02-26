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
	bool prevSelected = false;
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
	ofParameter<int> mPopulationSize = ofParameter<int>("Population size", 20, 10, 40);
	ofParameter<float> mMutationRate = ofParameter<float>("Mutation rate", 0.3, 0.0, 1.0);
	ofParameter<float> mMutationAmount = ofParameter<float>("Mutation amount", 0.4, 0.0, 1.0);

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

	void minorParameterChanged(float &val);
	void majorParameterChanged(int &val);

	void drawGenotype(vector<float> values);

	string getName();
	void keyPressed(int key);

};

