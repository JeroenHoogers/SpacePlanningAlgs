#pragma once

#include "SharedData.h"
#include "ofxState.h"
#include "ofxGui.h"
#include "ofxEasyCam.h"
#include "GeneticAlgorithm.h"
#include "Building.h"
#include "Measurements.h"
#include "ArchitectureProgram.h"
#include "ExteriorEvolver.h"
#include "InteriorEvolver.h"

#define ArchitectureState_StateName "ArchitectureState"

//struct Mass
//{
//	int x1;
//	int x2;
//	int z1;
//	int z2;
//	int floors; // 0,1,2
//
//	Mass()
//	{
//	};
//
//	Mass(int _x1, int _x2, int _z1, int _z2, int _floors)
//	{
//		x1 = _x1;
//		x2 = _x2;
//		z1 = _z1;
//		z2 = _z2;
//		floors = _floors;
//	};
//
//	ofPoint getPoint()
//	{
//		return ofPoint(
//			(x1 + x2) / 2.0f,
//			floors * 2,
//			(z1 + z2) / 2.0f);
//	};
//
//	ofPoint getSize()
//	{
//		return ofPoint(
//			fabsf(x1 - x2),
//			floors * 4,
//			fabsf(z1 - z2));
//	};
//};
//
//struct MassModel
//{
//	Mass m1;
//	Mass m2;
//	Mass m3;
//};

enum struct EEvolutionStep
{
	Exterior,
	Interior,
	Elements
};


class ArchitectureState : public itg::ofxState<SharedData>
{
private:
	EEvolutionStep currentStep;
	ofLight light;
	ofxEasyCam camera;

	float targetArea;
	int minStories = 1;
	int maxStories = 3;

	int visibleFloor = -1;

	bool mShowGui;

	ofPoint mousePos;

	ArchitectureProgram* pProgram;

	ofPolyline lotPolygon;

	ofxPanel gui;

	// evolution parameters
	ofParameter<int> mDimensions = ofParameter<int>("Num of params", 5, 1, 20);
	ofParameter<int> mPopulationSize = ofParameter<int>("Population size", 20, 10, 40);
	ofParameter<float> mMutationRate = ofParameter<float>("Mutation rate", 0.2, 0.0, 1.0);
	ofParameter<float> mMutationAmount = ofParameter<float>("Mutation amount", 0.4, 0.0, 1.0);
	//ofParameter<bool> mGroupGenes = ofParameter<bool>("Group mass genes", false, false, true);
	//ofParameter<bool> mDiversify = ofParameter<bool>("Diversify", false, false, true);

	ofParameter<int> mTilesHorizontal = ofParameter<int>("Horizontal Tiles", 4, 2, 5);
	ofParameter<int> mTilesVertical = ofParameter<int>("Vertical Tiles", 3, 2, 5);

	// architectural parameters
	ofParameter<int> mMaxExtrusions = ofParameter<int>("Num of Extrusions", 4, 1, 10);

	ofParameter<bool> mCrossoverInterpolate = ofParameter<bool>("Mating: Interpolate", false, false, true);
	ofParameter<bool> mCrossoverSwitch = ofParameter<bool>("Mating: Crossover", true, false, true);
	ofParameter<bool> mCrossoverGene = ofParameter<bool>("Mating: Pick random gene", false, false, true);

	ofxButton generateNextGenButton;


	//ofxPanel programmeGui;
	//ofParameter<int> mInhabitants = ofParameter<int>("Num of Inhabitants", 4, 1, 7);
	//ofParameter<int> mStories = ofParameter<int>("Num of Stories", 2, 1, 3);
	//vector<MassModel> massModels;
	//MassModel testModel;

	vector<int> selectedIndices;

	ExteriorEvolver exteriorEvolver;
	InteriorEvolver interiorEvolver;
	Evolver* pCurrentEvolver;

	bool initialized = false;

public:
	ArchitectureState();
	~ArchitectureState();

	void stateEnter();
	void stateExit();
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

	void setup();
	void setupEvolution();

	void update();
	void draw();
	void drawGUI();
	void drawTile(ofRectangle viewport, int index);
	void drawExteriorTile(ofRectangle viewport, int index);
	void drawInteriorTile(ofRectangle viewport, int index);

	void gotoNextStep();

//	Building convertGenotype(Genotype genotype);

	string getName();
	void keyPressed(int key);

	void toggleSelected(int index);

	void generateButtonPressed();

	void minorParameterChanged(float &val);
	void majorParameterChanged(int &val);

	void boolParameterChanged(bool &val);

	void matingModeChanged(bool &val);

	void evaluateCandidates();
};
