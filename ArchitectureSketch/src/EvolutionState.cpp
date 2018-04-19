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
	// button events
	generateNextGenButton.addListener(this, &EvolutionState::generateButtonPressed);

	// major parameters (require genetic algorithm reset)
	mDimensions.addListener(this, &EvolutionState::majorParameterChanged);
	mPopulationSize.addListener(this, &EvolutionState::majorParameterChanged);

	// minor parameters (do not require GA reset)
	mMutationRate.addListener(this, &EvolutionState::minorParameterChanged);
	mMutationAmount.addListener(this, &EvolutionState::minorParameterChanged);

	gui.setup("GA Settings");
	gui.add(mDimensions);
	gui.add(mPopulationSize);
	gui.add(mMutationRate);
	gui.add(mMutationAmount);
	gui.add(generateNextGenButton.setup("Generate offspring"));

	setupEvolution();
}
//--------------------------------------------------------------
void EvolutionState::setupEvolution()
{
	geneticAlg.setup(mPopulationSize.get(), mDimensions.get(), mMutationRate.get(), mMutationAmount.get());

	mTargets = geneticAlg.generateRandomDna();

	ofPoint offset = ofPoint(10, 60);

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

	int nrSelected = 0;

	// find selected indices
	for (int i = 0; i < mSelectionRectangles.size(); i++)
	{
		if (mSelectionRectangles[i].selected)
		{
			geneticAlg.select(mSelectionRectangles[i].index);
			nrSelected++;
		}

		mSelectionRectangles[i].selected = false;
	}

	// let the genetic algorithm generate offspring based on the selection
	geneticAlg.generateOffspring();

	// mark survivors from the last generation
	for (int i = 0; i < mSelectionRectangles.size(); i++)
	{
		if (i < nrSelected)
			mSelectionRectangles[i].prevSelected = true;
		else
			mSelectionRectangles[i].prevSelected = false;
	}
}

//--------------------------------------------------------------
void EvolutionState::majorParameterChanged(int& val)
{
	setupEvolution();
}

//--------------------------------------------------------------
void EvolutionState::minorParameterChanged(float& val)
{
	geneticAlg.mutationRate = mMutationRate.get();
	geneticAlg.mutationAmount = mMutationAmount.get();
}

//--------------------------------------------------------------
void EvolutionState::update()
{
}

//--------------------------------------------------------------
void EvolutionState::draw()
{
	// disable depth testing
	ofDisableDepthTest();

	// TODO: draw all phenotypes
	//ofBackgroundGradient(ofColor(200, 200, 200), ofColor(125, 125, 125));
	ofBackground(ofColor(190, 190, 190));

	ofSetColor(10);
	ofDrawBitmapStringHighlight("Generation: " + ofToString(geneticAlg.currentGeneration), 15, 25);
	
	for (int i = 0; i < mSelectionRectangles.size(); i++)
	{
		SelectionRect sr = mSelectionRectangles[i];
		ofSetColor(120);

		if(sr.mouseover)
			ofSetColor(150);

		if(sr.selected)
			ofSetColor(50, 160, 50);


		float diff = geneticAlg.absDifference(sr.index, mTargets);
		diff = roundf(diff * 100) / 100;

		ofFill();
		ofDrawRectangle(sr.rect);

		if (sr.prevSelected)
		{
			ofSetColor(50, 50, 50);
			ofNoFill();
			ofSetLineWidth(4.0);

			ofDrawRectangle(sr.rect);
		}
		
		ofPushMatrix();
		{
			ofSetColor(255);
			ofTranslate(sr.rect.x + 5, sr.rect.y + 5);

			ofSetColor(255);
			ofDrawBitmapStringHighlight(ofToString(i) + " | " + ofToString(diff), 0, -10);

			drawGenotype(geneticAlg.population[mSelectionRectangles[i].index].genes);
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

			ofSetLineWidth(3.0);
			// grey line inbetween
			ofSetColor(50, 50, 50);
			ofDrawLine(ofPoint(mTargets[i] * 100, 10), ofPoint(values[i] * 100, 10));

			// draw single param
			ofSetColor(50, 50, 255);
			ofDrawLine(ofPoint(values[i] * 100, 0), ofPoint(values[i] * 100, 20));

			ofSetColor(255, 50, 50);
			ofDrawLine(ofPoint(mTargets[i] * 100, 0), ofPoint(mTargets[i] * 100, 20));

		//	ofSetLineWidth(6.0);

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
