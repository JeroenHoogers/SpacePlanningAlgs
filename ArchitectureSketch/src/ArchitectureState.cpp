#include "ArchitectureState.h"

//--------------------------------------------------------------
ArchitectureState::ArchitectureState()
{
}

//--------------------------------------------------------------
ArchitectureState::~ArchitectureState()
{
}

//--------------------------------------------------------------
void ArchitectureState::stateEnter()
{

}

//--------------------------------------------------------------
void ArchitectureState::stateExit()
{

}

//--------------------------------------------------------------
void ArchitectureState::setup()
{
	// use AA
	ofEnableAntiAliasing();

	// use depth information for occlusion
	ofEnableDepthTest();

	// use normalized tex coords
	ofEnableNormalizedTexCoords();

	//glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);

	// load shader
	//phong.load("shaders/phong.vert", "shaders/phong.frag");

	// setup light
	light.enable();
	light.setDirectional();
	light.setAmbientColor(ofColor(25, 25, 40));
	light.setDiffuseColor(ofColor(210, 200, 180));

	light.setPosition(-150, -200, -400);
	light.lookAt(ofVec3f(0, 0, 0));

	ofSetLineWidth(1.5);

	camera.setDistance(40);
	camera.disableRoll();

	//post.init(ofGetWidth(), ofGetHeight());
	//post.createPass<FxaaPass>();
	////post.createPass<BloomPass>();
	//post.createPass<SSAOPass>();

	//testModel.m1 = Mass(-8, 8, -6, 6, 2);
	//testModel.m2 = Mass(-16, -8, -5, 5, 1);
	//testModel.m3 = Mass(8, 16, -5, 5, 1);

	// button events 
	generateNextGenButton.addListener(this, &ArchitectureState::generateButtonPressed);

	//// major parameters (require genetic algorithm reset)
	//mDimensions.addListener(this, &ArchitectureState::majorParameterChanged);
	//mPopulationSize.addListener(this, &ArchitectureState::majorParameterChanged);

	mTilesHorizontal.addListener(this, &ArchitectureState::majorParameterChanged);
	mTilesVertical.addListener(this, &ArchitectureState::majorParameterChanged);

	// minor parameters (do not require GA reset)
	mMutationRate.addListener(this, &ArchitectureState::minorParameterChanged);
	mMutationAmount.addListener(this, &ArchitectureState::minorParameterChanged);

	mCrossoverGene.addListener(this, &ArchitectureState::matingModeChanged);
	mCrossoverInterpolate.addListener(this, &ArchitectureState::matingModeChanged);
	mCrossoverSwitch.addListener(this, &ArchitectureState::matingModeChanged);

	mInhabitants.addListener(this, &ArchitectureState::numInhabitantsChanged);
	mStories.addListener(this, &ArchitectureState::numInhabitantsChanged);

	// setup gui 
	gui.setup("Evolution settings");
	//gui.add(mDimensions);
	//gui.add(mPopulationSize);

	gui.add(mTilesHorizontal);
	gui.add(mTilesVertical);

	gui.add(mMutationRate);
	gui.add(mMutationAmount);



	gui.add(mCrossoverSwitch);
	gui.add(mCrossoverInterpolate);
	gui.add(mCrossoverGene);
	//gui.add(mGroupGenes);
	//gui.add(mDiversify);
	gui.add(generateNextGenButton.setup("Generate offspring"));

	programmeGui.setup("Achitecture programme", "settings.xml", 250, 10);
	programmeGui.add(mInhabitants);
	programmeGui.add(mStories);

	int inhab = mInhabitants.get();
	numInhabitantsChanged(inhab);

	setupEvolution();

	mShowGui = true;
}

//--------------------------------------------------------------
void ArchitectureState::setupEvolution()
{
	// setup genetic algorithm
	geneticAlgorithm.setup(mTilesHorizontal.get() * mTilesVertical.get(), 15, mMutationRate.get(), mMutationAmount.get());

	buildings.clear();

	for (int i = 0; i < geneticAlgorithm.population.size(); i++)
	{
		Building building = Building();
		building.LoadFromGenotype(geneticAlgorithm.population[i]);
		building.GenerateBuilding();

		buildings.push_back(building);
	}
}

//--------------------------------------------------------------
void ArchitectureState::update()
{

}

//--------------------------------------------------------------
void ArchitectureState::draw()
{
	ofBackgroundGradient(ofColor(210, 210, 210), ofColor(170, 170, 170));

	int tilew = (ofGetWidth() / mTilesHorizontal.get());
	int tileh = (ofGetHeight() / mTilesVertical.get());

	for (int i = 0; i < (mTilesHorizontal.get() * mTilesVertical.get()); i++)
	{
		int x = (i % mTilesHorizontal.get()) * tilew;
		int y = (i / mTilesHorizontal.get()) * tileh;

		ofPushMatrix();
		{
			//ofTranslate(x, y);
			//ofScale(1.0f / tilew, 1.0f / tileh);

			//ofRectangle rect = ofRectangle(0, 0, tilew, tileh);
			ofRectangle rect = ofRectangle(x, y, tilew, tileh);
			
			drawTile(rect, i);
		}
		ofPopMatrix();
	}

	// draw tiles
	ofSetLineWidth(2);
	ofNoFill();
	ofSetColor(50);

	for (int i = 0; i < (mTilesHorizontal.get() * mTilesVertical.get()); i++)
	{
		// draw tiles
		int x = (i % mTilesHorizontal.get()) * tilew;
		int y = (i / mTilesHorizontal.get()) * tileh;

		ofDrawRectangle(x, y, 0, x + tilew, y + tileh);
	}
	//drawGUI();

	///
	camera.begin();
	camera.end();

	// show generation
	ofSetColor(10);
	ofDrawBitmapStringHighlight("Generation: " + ofToString(geneticAlgorithm.currentGeneration), 10, 20);
	ofDrawBitmapStringHighlight("Minimum area: " + ofToString(targetArea) + " m2", ofGetWidth() - 200, 20);
	ofDrawBitmapStringHighlight("R: Generate next gen,   H: Hide gui", ofPoint(ofGetWidth() * 0.5f - 150, 20));

	if (mShowGui)
	{
		gui.draw();
		programmeGui.draw();
	}
}

//--------------------------------------------------------------
void ArchitectureState::drawTile(ofRectangle viewport, int index)
{
	// show selection
	for (int i = 0; i < selectedIndices.size(); i++)
	{
		if (selectedIndices[i] == index)
		{
			ofFill();
			ofSetColor(50, 180, 50, 80);
			ofDrawRectangle(viewport);
			break;
		}
	}

	ofSetLineWidth(1.5);
	ofEnableDepthTest();

	light.setPosition(-camera.getGlobalPosition());
	light.lookAt(ofVec3f(0, 0, 0));

	camera.begin(viewport);

	// draw outlines
	ofNoFill();
	//ofSetColor(140);

	ofEnableLighting();
	//phong.begin();

	// draw shapes
	ofSetColor(220);
	ofFill();

	//// draw mass 1
	////ofPushMatrix();
	////{
	//ofPoint pos;
	//ofPoint sz;

	buildings[index].draw();

	// draw scale model
	ofSetColor(30);
	ofDrawCylinder(buildings[index].boundingBox.getMinX() - 1.0f, 0.9f, 0, 0.3f, 1.8f);
	ofDrawCylinder(buildings[index].boundingBox.getMaxX() + 1.0f, 0.9f, 0, 0.3f, 1.8f);
	ofDrawCylinder(0, 0.9f, buildings[index].boundingBox.getMinY() - 1.0f, 0.3f, 1.8f);
	ofDrawCylinder(0, 0.9f, buildings[index].boundingBox.getMaxY() + 1.0f, 0.3f, 1.8f);

	//ofDrawBox(0, 0.85f, 0, 0.5f, 1.7f, 0.5f);


	//}
	//ofPopMatrix();

	//ofPushMatrix();
	//{
	//	ofSetColor(80, 80, 180, 255);
	//	ofTranslate(12, 3);
	//	ofDrawBox(8, 6, 12);
	//}
	//ofPopMatrix();

	//ofPushMatrix();
	//{
	//	ofSetColor(80, 180, 80, 255);
	//	ofTranslate(-12, 3);
	//	ofDrawBox(8, 6, 12);
	//}
	//ofPopMatrix();

	ofDisableLighting();
	//phong.end();

	ofPushMatrix();
	{
		//ofRotateX(-90);
		ofRotateZ(90);
		ofSetLineWidth(1);
		ofSetColor(120);
		//ofDrawPlane(0, 0, 50, 50);
		ofDrawGridPlane(1, 20);
	}
	ofPopMatrix();

	ofDisableDepthTest();


	//ofDrawAxis(5);
	//post.end();

	camera.end();

	ofPoint p = viewport.getBottomLeft() + ofVec2f(5, -5);
	
	ofColor bgCol = ofColor::gray;
	if (buildings[index].GetTotalArea() >= targetArea)
		bgCol = ofColor::forestGreen;

	ofDrawBitmapStringHighlight("Area: " + ofToString(buildings[index].GetTotalArea()) + " m2", p, bgCol);
}

//--------------------------------------------------------------
void ArchitectureState::drawGUI()
{
	ofSetColor(255);

	string controls = "Controls: \n(o): toggle orthographic\n(t): top view \n(f): front view \n(s): side view";
	ofDrawBitmapStringHighlight(controls, ofPoint(10, 20));

	//ofTranslate(10, ofGetWidth() - 140);

	ofDrawBitmapStringHighlight("fps: " + ofToString(ofGetFrameRate()), ofPoint(ofGetWidth() - 110, 20));
}

//--------------------------------------------------------------
string ArchitectureState::getName()
{
	return ArchitectureState_StateName;
}

//--------------------------------------------------------------
void ArchitectureState::keyPressed(int key)
{
	float dist = camera.getDistance();

	// top view
	if (key == 't')
	{
		camera.setPosition(0, dist, 0);
		camera.lookAt(ofPoint(0, 0, 0));
		//camera.setScale(0.5);

	}

	// front view
	if (key == 'f')
	{
		camera.setPosition(0, 0, dist);
		camera.lookAt(ofPoint(0, 0, 0));
	}

	// side view
	if (key == 's')
	{
		camera.setPosition(dist, 0, 0);
		camera.lookAt(ofPoint(0, 0, 0));
	}

	// toggle orthographic
	if (key == 'o')
	{
		if (!camera.getOrtho())
		{
			camera.setScale(0.1, 0.1, 0.1);
			camera.enableOrtho();
		}
		else
		{
			//camera.setScale(0.1, 0.1, 0.1);
			camera.disableOrtho();
		}
	}

	// toggle gui
	if (key == 'h')
	{
		mShowGui = !mShowGui;
	}

	if (key == 'r')
	{
		generateButtonPressed();
	}
}

//--------------------------------------------------------------
void ArchitectureState::mousePressed(int x, int y, int button)
{
	if (button == 0)
		mousePos = ofPoint(x, y);
}

//--------------------------------------------------------------
void ArchitectureState::mouseReleased(int x, int y, int button)
{
	if (button == 0)
	{
		// if mouse hasn't moved do selection
		if (mousePos != ofPoint(x, y))
			return;

		// calculate tile dimensions
		int tilew = (ofGetWidth() / mTilesHorizontal.get());
		int tileh = (ofGetHeight() / mTilesVertical.get());

		// calculate index based on mouse position
		int index = (x / tilew) + (y / tileh) * mTilesHorizontal.get();
		bool removed = false;

		if (index < geneticAlgorithm.populationSize)
		{
			for (int i = 0; i < selectedIndices.size(); i++)
			{
				// remove index
				if (selectedIndices[i] == index)
				{
					selectedIndices.erase(selectedIndices.begin() + i);
					removed = true;
					break;
				}
			}

			if (!removed)
				selectedIndices.push_back(index);
		}
	}
}

//--------------------------------------------------------------
void ArchitectureState::generateButtonPressed()
{
	cout << "current generation: " << geneticAlgorithm.currentGeneration << endl;

	// find selected indices
	for (int i = 0; i < selectedIndices.size(); i++)
	{
		geneticAlgorithm.select(selectedIndices[i]);
	}

	if (selectedIndices.size() > 0)
	{
		// let the genetic algorithm generate offspring based on the selection
		geneticAlgorithm.generateOffspring();
	}
	else
	{
		setupEvolution();
	}

	//// mark survivors from the last generation
	//for (int i = 0; i < mSelectionRectangles.size(); i++)
	//{
	//	if (i < nrSelected)
	//		mSelectionRectangles[i].prevSelected = true;
	//	else
	//		mSelectionRectangles[i].prevSelected = false;
	//}

	//massModels.clear();

	// generate phenotypes for the new population
	for (int i = 0; i < geneticAlgorithm.population.size(); i++)
	{
		//for (int j = 0; j < 100; j++)
		//{
			buildings[i].LoadFromGenotype(geneticAlgorithm.population[i]);

		//	// force area to be at least minimum area
		//	if (buildings[i].GetTotalArea() >= targetArea)
		//		break;
		//}

		buildings[i].GenerateBuilding();
	}

	selectedIndices.clear();
}

//--------------------------------------------------------------
void ArchitectureState::majorParameterChanged(int& val)
{
	setupEvolution();
}

//--------------------------------------------------------------
void ArchitectureState::minorParameterChanged(float& val)
{
	geneticAlgorithm.mutationRate = mMutationRate.get();
	geneticAlgorithm.mutationAmount = mMutationAmount.get();
}

//--------------------------------------------------------------
void ArchitectureState::boolParameterChanged(bool &val)
{
	// TODO: add boolean params
	//geneticAlgorithm.groupGenes = mGroupGenes.get();
}

//--------------------------------------------------------------
void ArchitectureState::numInhabitantsChanged (int &val)
{
	// From the metric planning handbook

	int inhabitants = mInhabitants.get();
	int stories = mStories.get();

	switch (inhabitants)
	{
	case 1:
		targetArea = 33.0f;

		if (stories > 1)
			targetArea = -1;
		break;
	case 2:
		targetArea = 48.5f;

		if (stories > 1)
			targetArea = -1;
		break;
	case 3:
		targetArea = 61.0f;

		if (stories > 1)
			targetArea = -1;
		break;
	case 4:
		if (stories == 1)
			targetArea = 71.5;
		else if (stories == 2)
			targetArea = 76.5f;
		else
			targetArea = -1;

		break;
	case 5:

		if (stories == 1)
			targetArea = 71.5;
		else if (stories == 2)
			targetArea = 86.5f;
		else
			targetArea = 98.5f;
		break;
	case 6:
		if (stories == 1)
			targetArea = 88.5f;
		else if (stories == 2)
			targetArea = 97.0f;
		else
			targetArea = 102.0f;

		break;
	case 7:
		if (stories == 2)
			targetArea = 114.5f;
		else if (stories == 3)
			targetArea = 118.5f;
		else
			targetArea = -1;
		break;
	default:
		break;
	}

	// round area
	//targetArea = roundf(targetArea * 100) / 100;

	// TODO: add boolean params
	//geneticAlgorithm.groupGenes = mGroupGenes.get();
}


//--------------------------------------------------------------
void ArchitectureState::matingModeChanged(bool &val)
{

	if (val)
	{
		// deselect others
		mCrossoverInterpolate.set(false);
		mCrossoverGene.set(false);
		mCrossoverSwitch.set(false);

		val = true;

		// change mating mode
		if (mCrossoverSwitch.get())
			geneticAlgorithm.matingStrat = MatingStrategy::SwitchSource;

		if (mCrossoverInterpolate.get())
			geneticAlgorithm.matingStrat = MatingStrategy::Interpolate;

		if (mCrossoverGene.get())
			geneticAlgorithm.matingStrat = MatingStrategy::Gene;

	}

	// TODO: add boolean params
	//geneticAlgorithm.groupGenes = mGroupGenes.get();
}