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
	light.enable();

	int inhabitants = pProgram->inhabitants;
	int stories = pProgram->stories;

	targetArea = Measurements::getMinimumArea(inhabitants, stories);

	// Update lot polygon
	lotPolygon.clear();

	lotPolygon.addVertex(
		ofPoint(-pProgram->lotWidth * 0.5f, 0, -pProgram->lotDepth * 0.5f));
	lotPolygon.addVertex(
		ofPoint(-pProgram->lotWidth * 0.5f, 0, pProgram->lotDepth * 0.5f));
	lotPolygon.addVertex(
		ofPoint(pProgram->lotWidth * 0.5f, 0, pProgram->lotDepth * 0.5f));
	lotPolygon.addVertex(
		ofPoint(pProgram->lotWidth * 0.5f, 0, -pProgram->lotDepth * 0.5f));
	lotPolygon.close();


	evaluateCandidates();
}

//--------------------------------------------------------------
void ArchitectureState::stateExit()
{
	light.disable();
}

//--------------------------------------------------------------
void ArchitectureState::setup()
{
	// use AA
	//ofEnableAntiAliasing();

	// use depth information for occlusion
//	ofEnableDepthTest();
	// use normalized tex coords
	ofEnableNormalizedTexCoords();

	//glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);

	// load shader
	//phong.load("shaders/phong.vert", "shaders/phong.frag");

	// setup light
	//light.enable();
	light.setDirectional();
	light.setAmbientColor(ofColor(60, 60, 80));
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

	//programmeGui.setup("Achitecture program", "settings.xml", 250, 10);
	//programmeGui.add(mInhabitants);
	//programmeGui.add(mStories);

//	int inhab = mInhabitants.get();
//	numInhabitantsChanged(inhab);

	mShowGui = false;

	pProgram = &getSharedData().program;

	setupEvolution();
}

//--------------------------------------------------------------
void ArchitectureState::setupEvolution()
{
	// setup genetic algorithm
	int tiles = mTilesHorizontal.get() * mTilesVertical.get();
	geneticAlgorithm.setup(500, 19, mMutationRate.get(), mMutationAmount.get());

	buildings.clear();

	for (int i = 0; i < geneticAlgorithm.population.size(); i++)
	{
		Building building = Building();
	//	building.LoadFromGenotype(geneticAlgorithm.population[i]);
	//	building.GenerateBuilding();

		buildings.push_back(building);
	}

	candidates.clear();

	// evaluate all candidates
	evaluateCandidates();
}

//--------------------------------------------------------------
void ArchitectureState::update()
{

}

//--------------------------------------------------------------
void ArchitectureState::draw()
{
	light.setPosition(-camera.getGlobalPosition() + ofVec3f(0,2,0));
	light.lookAt(ofVec3f(0, 0, 0));

//	ofBackgroundGradient(ofColor(210, 210, 210), ofColor(170, 170, 170));
	ofBackground(ofColor(200));

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
	
	// Make sure the camera can be interacted with
	camera.begin();
	camera.end();

	// show generation
	ofSetColor(10);
	ofDrawBitmapStringHighlight("Generation: " + ofToString(geneticAlgorithm.currentGeneration), 10, 20);
	ofDrawBitmapStringHighlight("Minimum area: " + ofToString(targetArea) + " m2", ofGetWidth() - 200, 20);
	ofDrawBitmapStringHighlight("R: Generate next gen,   H: Hide gui", ofPoint(ofGetWidth() * 0.5f - 150, 20));

	//ofDrawBitmapStringHighlight("Inhabs: " + ofToString(pProgram->inhabitants), ofGetWidth() - 200, 20);
	if (mShowGui)
	{
		gui.draw();
	}
}

//--------------------------------------------------------------
void ArchitectureState::drawTile(ofRectangle viewport, int tileIndex)
{
	if (tileIndex >= candidates.size())
		return;

	int index = candidates[tileIndex];

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

	camera.begin(viewport);

	// draw outlines
	ofNoFill();
	//ofSetColor(140);

	ofEnableLighting();
	//phong.begin();

	// draw shapes
	ofSetColor(255);
	ofFill();

	buildings[index].draw();

	// draw scale model
	ofSetColor(30);
	ofRectangle boundingRect = buildings[index].GetFloorBoundingBox(0);

	ofDrawCylinder(boundingRect.getMinX() - 1.0f, 0.9f, 0, 0.2f, 1.8f);
	ofDrawCylinder(boundingRect.getMaxX() + 1.0f, 0.9f, 0, 0.2f, 1.8f);
	ofDrawCylinder(0, 0.9f, boundingRect.getMinY() - 1.0f, 0.2f, 1.8f);
	ofDrawCylinder(0, 0.9f, boundingRect.getMaxY() + 1.0f, 0.2f, 1.8f);

	//ofDrawBox(0, 0.85f, 0, 0.5f, 1.7f, 0.5f);
	//}
	//ofPopMatrix();

	ofDisableLighting();
	//phong.end();

	// HACK: hack depth buffer range to make sure the lines render on top of the geometry
	glDepthRange(0.0, 0.9995);

	// draw lot
	ofSetColor(100);
	ofSetLineWidth(2.0f);
	ofNoFill();
	lotPolygon.draw();

	// HACK: hack depth buffer range to make sure the lines render on top of the geometry
	glDepthRange(0.0005, 1.0);
	
	ofPushMatrix();
	{
		//ofRotateX(-90);
		ofRotateZ(90);
		ofSetLineWidth(0.8f);
		ofSetColor(170);
		//ofDrawPlane(0, 0, 100, 100);
		ofDrawGridPlane(0.5f, 30);
	}
	ofPopMatrix();

	glDepthRange(0.0, 1.0);

	// disable depth testing
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
		int tileIndex = (x / tilew) + (y / tileh) * mTilesHorizontal.get();
		bool removed = false;

		if (tileIndex < candidates.size())
		{
			int index = candidates[tileIndex];

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
	evaluateCandidates();
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


//--------------------------------------------------------------
bool ArchitectureState::isBuildingValid(Building& building)
{
	int stories = pProgram->stories;
	int inhabitants = pProgram->inhabitants;

	float minimumArea = Measurements::getMinimumArea(inhabitants, stories);

	// area check
	if (building.GetTotalArea() < minimumArea)
		return false;

	ofRectangle lotRect = pProgram->getLotRectangle();

	for (size_t i = 0; i < building.floorShapes.size(); i++)
	{
		ofRectangle bb = building.floorShapes[i].getBoundingBox();
		ofRectangle intersection = lotRect.getIntersection(bb);

		// check whether the mass is inside the bounds of the plot
		if (intersection != bb)
			return false;

		// check for invalid (self-intersecting) geometry
		if (MeshHelper::hasSelfIntersections(building.floorShapes[i]))
			return false;
	}

	return true;
}

//--------------------------------------------------------------
void ArchitectureState::evaluateCandidates()
{
	cout << "current generation: " << geneticAlgorithm.currentGeneration << endl;

	candidates.clear();

	//// mark survivors from the last generation
	//for (int i = 0; i < mSelectionRectangles.size(); i++)
	//{
	//	if (i < nrSelected)
	//		mSelectionRectangles[i].prevSelected = true;
	//	else
	//		mSelectionRectangles[i].prevSelected = false;
	//}

	//massModels.clear();
	int tiles = mTilesHorizontal.get() * mTilesVertical.get();
	bool done = false;

	//while(!done)
	//{
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
			geneticAlgorithm.generateRandomPopulation();
			//setupEvolution();
		}

		// generate phenotypes for the new population
		for (int i = 0; i < geneticAlgorithm.population.size(); i++)
		{
			//for (int j = 0; j < 100; j++)
			//{
			buildings[i].LoadFromGenotype(geneticAlgorithm.population[i]);

			// evaluate candidate
			if (isBuildingValid(buildings[i]))
			{
				candidates.push_back(i);
				buildings[i].GenerateBuilding();
			}


			//	// force area to be at least minimum area
			//	if (buildings[i].GetTotalArea() >= targetArea)
			//		break;
			//}

			//if()
			//buildings[i].GenerateBuilding();
		}

		// generate building for candidates
		// Inifinite loop potential
		//if (candidates.size() >= tiles)
		//{
			//for (int i = 0; i < candidates.size(); i++)
			//{
			//	buildings[candidates[i]].GenerateBuilding();
			//}

			//done = true;
			selectedIndices.clear();
		//}

		// do a subselection of buildings
	//}
}