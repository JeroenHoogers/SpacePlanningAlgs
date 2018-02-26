#include "EvolutionState.h"

//--------------------------------------------------------------
EvolutionState::EvolutionState()
{
}

//--------------------------------------------------------------
EvolutionState::~EvolutionState()
{
}

//--------------------------------------------------------------
void EvolutionState::stateEnter()
{

}

//--------------------------------------------------------------
void EvolutionState::stateExit()
{

}

//--------------------------------------------------------------
void EvolutionState::setup()
{	
	generateNextGenButton.addListener(this, &EvolutionState::generateButtonPressed);

	gui.setup();
	gui.add(mDimensions);
	gui.add(mMutationRange);
	gui.add(mMutationProbability);
	gui.add(generateNextGenButton.setup("Generate offspring"));

	setupEvolution();
}
//--------------------------------------------------------------
void EvolutionState::setupEvolution()
{
	geneticAlg.setup(20, 5);

	mTargets = geneticAlg.generateRandomDna();

	ofPoint offset = ofPoint(10, 30);

	mSelectionRectangles.clear();

	// rect height
	float h = mDimensions.get() * 30 + 10;
	int cols = 10;

	for (int i = 0; i < geneticAlg.population.size(); i++)
	{
		// initialize rectangles
		SelectionRect sr = SelectionRect();
		sr.index = i;
		sr.rect = ofRectangle(offset.x + (i % cols)  * 120 , offset.y + floorf(i / cols) * (h + 25), 110, h);

		mSelectionRectangles.push_back(sr);
	}
}

//--------------------------------------------------------------
void EvolutionState::generateButtonPressed()
{
	cout << "current generation: " << geneticAlg.currentGeneration << endl;

	// find selected indices
	for (int i = 0; i < mSelectionRectangles.size(); i++)
	{
		if (mSelectionRectangles[i].selected)
			geneticAlg.select(mSelectionRectangles[i].index);

		mSelectionRectangles[i].selected = false;
	}

	// let the genetic algorithm generate offspring based on the selection
	geneticAlg.generateOffspring();
}

//--------------------------------------------------------------
void EvolutionState::update()
{
}

//--------------------------------------------------------------
void EvolutionState::draw()
{
	ofDisableDepthTest();

	// TODO: draw all phenotypes
	ofBackgroundGradient(ofColor(200, 200, 200), ofColor(125, 125, 125));
	
	
	for (int i = 0; i < mSelectionRectangles.size(); i++)
	{
		SelectionRect sr = mSelectionRectangles[i];
		ofSetColor(100);

		if(sr.mouseover)
			ofSetColor(120);

		if(sr.selected)
			ofSetColor(50, 100, 50);

		ofDrawRectangle(sr.rect);

		ofPushMatrix();
		{
			ofSetColor(255);
			ofTranslate(sr.rect.x + 5, sr.rect.y + 5);

			ofSetColor(255);
			ofDrawBitmapStringHighlight(ofToString(i), 0, -10);

			drawGenotype(geneticAlg.population[mSelectionRectangles[i].index]);
		}
		ofPopMatrix();
	}


	//ofTranslate(10, 150);
	//for (int i = 0; i < geneticAlg.population.size(); i++)
	//{
	////	if((i+1) * 120 + 10 > ofGetWidth() && )
	//			
	//	if (i == ceil(geneticAlg.population.size() / 2))
	//		ofTranslate(-i * 120, mDimensions.get() * 30 + 35);
	//			
	//	ofSetColor(100);
	//	//ofDrawRectangle(-5, -5, 110, mDimensions.get() * 30 + 10);
	//	drawGenotype(geneticAlg.population[i]);
	//	ofTranslate(120, 0);
	//}

	gui.draw();
}

//--------------------------------------------------------------
void EvolutionState::drawGenotype(vector<float> values)
{
	// check if the amount of parameters matches
	if (mDimensions.get() != values.size() || mDimensions.get() != mTargets.size())
		return;

	ofPushMatrix();
	{

		// TODO: draw a single phenotype
		for (int i = 0; i < mDimensions.get(); i++)
		{
			ofSetLineWidth(2.0);
			ofSetColor(255);
			
			ofDrawLine(ofPoint(0, 0), ofPoint(0,20));
			ofDrawLine(ofPoint(100, 0), ofPoint(100, 20));
			ofDrawLine(ofPoint(0, 10), ofPoint(100, 10));

			// draw single param
			ofSetColor(50, 50, 255);
			ofDrawLine(ofPoint(values[i] * 100, 0), ofPoint(values[i] * 100, 20));

			ofSetColor(255, 50, 50);
			ofDrawLine(ofPoint(mTargets[i] * 100, 0), ofPoint(mTargets[i] * 100, 20));

			ofSetLineWidth(6.0);
			ofSetColor(50, 50, 50);

			ofDrawLine(ofPoint(mTargets[i] * 100, 10), ofPoint(values[i] * 100, 10));

			ofTranslate(0, 30);
		}
	}
	ofPopMatrix();
}


//--------------------------------------------------------------
string EvolutionState::getName()
{
	return EvolutionState_StateName;
}

//--------------------------------------------------------------
void EvolutionState::keyPressed(int key)
{

}

//--------------------------------------------------------------
void EvolutionState::mousePressed(int x, int y, int button)
{
	// click selection
	for (int i = 0; i < mSelectionRectangles.size(); i++)
	{
		if (mSelectionRectangles[i].rect.inside(x, y))
			mSelectionRectangles[i].selected = !mSelectionRectangles[i].selected;
	}
}

//--------------------------------------------------------------
void EvolutionState::mouseMoved(int x, int y)
{
	// update mouseover
	for (int i = 0; i < mSelectionRectangles.size(); i++)
	{
		mSelectionRectangles[i].mouseover = mSelectionRectangles[i].rect.inside(x, y);
	}
}
