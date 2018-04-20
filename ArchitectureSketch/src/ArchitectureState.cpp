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

	pCurrentEvolver->generate(selectedIndices);
	//evaluateCandidates();
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

	// create evolvers
	int tiles = mTilesHorizontal.get() * mTilesVertical.get();
	exteriorEvolver.setup(tiles, pProgram);
	interiorEvolver.setup(tiles, pProgram);

	currentStep = EEvolutionStep::Exterior;
	pCurrentEvolver = &exteriorEvolver;
	//interiorEvolver.setup()

	//setupEvolution();
}

////--------------------------------------------------------------
//void ArchitectureState::setupEvolution()
//{
//	// setup genetic algorithm
//	int tiles = mTilesHorizontal.get() * mTilesVertical.get();
//	geneticAlgorithm.setup(1000, 19, mMutationRate.get(), mMutationAmount.get());
//
//	buildings.clear();
//
//	for (int i = 0; i < geneticAlgorithm.population.size(); i++)
//	{
//		Building building = Building();
//	//	building.LoadFromGenotype(geneticAlgorithm.population[i]);
//	//	building.GenerateBuilding();
//
//		buildings.push_back(building);
//	}
//
//	candidates.clear();
//
//	// evaluate all candidates
//	evaluateCandidates();
//}

//--------------------------------------------------------------
void ArchitectureState::update()
{
	//if (currentStep == EEvolutionStep::Interior)
	//	interiorEvolver.optimizeInteriors();
}

//--------------------------------------------------------------
void ArchitectureState::draw()
{
	light.setPosition(-camera.getGlobalPosition() + ofVec3f(0,2,0));
	light.lookAt(ofVec3f(0, 0, 0));

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


	if (showDebug)
	{	
		// calculate index based on mouse position
		int tileIndex = floorf(mousePos.x / tilew) + floorf(mousePos.y / tileh) * mTilesHorizontal.get();

		// draw debug menu
		pCurrentEvolver->drawDebug(mousePos, tileIndex);
	}
	
	// Make sure the camera can be interacted with
	camera.begin();
	camera.end();

	// show generation
	ofSetColor(10);
	ofDrawBitmapStringHighlight("Generation: " + ofToString(pCurrentEvolver->generation), 10, 20);
	ofDrawBitmapStringHighlight("Minimum area: " + ofToString(targetArea) + " m2", ofGetWidth() - 200, 20);
	ofDrawBitmapStringHighlight("R: Generate next gen,  A: Accept design", ofPoint(ofGetWidth() * 0.5f - 150, 20));

	//ofDrawBitmapStringHighlight("Inhabs: " + ofToString(pProgram->inhabitants), ofGetWidth() - 200, 20);
	if (mShowGui)
	{
		gui.draw();
	}
}

//--------------------------------------------------------------
void ArchitectureState::drawTile(ofRectangle viewport, int tileIndex)
{
	// draw green background to show selection
	for (int i = 0; i < selectedIndices.size(); i++)
	{
		if (selectedIndices[i] == tileIndex)
		{
			ofFill();
			ofSetColor(50, 180, 50, 80);
			ofDrawRectangle(viewport);
			break;
		}
	}

	switch (currentStep)
	{
	case EEvolutionStep::Exterior:
		drawExteriorTile(viewport, tileIndex);
		break;
	case EEvolutionStep::Interior:
		drawInteriorTile(viewport, tileIndex);
		break;
	case EEvolutionStep::Elements:
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ArchitectureState::drawExteriorTile(ofRectangle viewport, int index)
{
	Building* pBuilding = exteriorEvolver.getBuildingAt(index);

	if (pBuilding == NULL)
		return;

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

	pBuilding->draw(visibleFloor);

	// draw scale model
	ofSetColor(30);
	ofRectangle boundingRect = pBuilding->GetFloorBoundingBox(0);

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
	ofSetColor(80);
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

	ofFill();
	ofSetColor(20);
	ofDrawArrow(ofVec3f(0, 1, -18), ofVec3f(0, 1, -15), 0.3f);

	// disable depth testing
	ofDisableDepthTest();


	//ofDrawAxis(5);
	//post.end();

	camera.end();

	ofPoint p = viewport.getBottomLeft() + ofVec2f(5, -5);

	ofColor bgCol = ofColor::gray;
	if (pBuilding->GetTotalArea() >= targetArea)
		bgCol = ofColor::forestGreen;

	ofDrawBitmapStringHighlight("Area: " + ofToString(pBuilding->GetTotalArea()) + " m2", p, bgCol);
}

//--------------------------------------------------------------
void ArchitectureState::drawInteriorTile(ofRectangle viewport, int index)
{
	vector<InteriorRoom>* pInterior = interiorEvolver.getInteriorAt(index);

	// get lot rectangle
	ofRectangle lot = pProgram->getLotRectangle();

	// calculate ratio
	float ratioW = viewport.width / lot.width;
	float ratioH = viewport.height / lot.height;

	float ratio = fminf(ratioW, ratioH);

	ofPushMatrix();
	{
		ofNoFill();
		ofSetColor(0);
		
		ofTranslate(viewport.x + viewport.width * 0.5f, viewport.y + viewport.height * 0.5f);
		ofScale(ratio, ratio);
		// draw floor outline
		ofSetLineWidth(3.0f);

		interiorEvolver.floorshape.draw();

		// draw rooms
		ofSetColor(40);
		ofSetLineWidth(1.5f);

		if (pInterior != NULL)
		{
			for (int i = 0; i < pInterior->size(); i++)
			{
				ofFill();
				
				(*pInterior)[i].shape.draw();
				
				// draw room name and area
				float area = roundf((*pInterior)[i].getArea() * 10) / 10;
				ofPoint p = (*pInterior)[i].shape.getCentroid2D() + ofPoint(-5, -10) / ratio;
				ofDrawBitmapString((*pInterior)[i].pRoom->code + "\n" + ofToString(area) + " m2", p);
			}
		}

		// TODO: draw rooms
	}
	ofPopMatrix();
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

	if (key == 'q')
	{
		showDebug = !showDebug;
	}

	// visible floor down
	if (key == '[')
	{
		if (visibleFloor > -1)
			visibleFloor--;
	}

	// visible floor up
	if (key == ']')
	{
		if (visibleFloor < pProgram->stories - 1)
			visibleFloor++;
	}

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

	if (key == 'a')
	{
		gotoNextStep();
	}
}

//--------------------------------------------------------------
void ArchitectureState::mousePressed(int x, int y, int button)
{
	if (button == 0)
		clickPos = ofPoint(x, y);
}

//--------------------------------------------------------------
void ArchitectureState::mouseMoved(int x, int y)
{
	mousePos = ofPoint(x, y);
}

//--------------------------------------------------------------
void ArchitectureState::mouseReleased(int x, int y, int button)
{
	if (button == 0)
	{
		// if mouse hasn't moved do selection
		if (clickPos != ofPoint(x, y))
			return;

		// calculate tile dimensions
		int tilew = (ofGetWidth() / mTilesHorizontal.get());
		int tileh = (ofGetHeight() / mTilesVertical.get());

		// calculate index based on mouse position
		int tileIndex = (x / tilew) + (y / tileh) * mTilesHorizontal.get();
		bool removed = false;


		for (int i = 0; i < selectedIndices.size(); i++)
		{
			// remove index
			if (selectedIndices[i] == tileIndex)
			{
				selectedIndices.erase(selectedIndices.begin() + i);
				removed = true;
				break;
			}
		}

		if (!removed)
			selectedIndices.push_back(tileIndex);
	}
}

//--------------------------------------------------------------
void ArchitectureState::generateButtonPressed()
{
	pCurrentEvolver->generate(selectedIndices);
	selectedIndices.clear();
	//evaluateCandidates();
}

//--------------------------------------------------------------
void ArchitectureState::majorParameterChanged(int& val)
{
//	setupEvolution();
}

//--------------------------------------------------------------
void ArchitectureState::minorParameterChanged(float& val)
{
	// TODO: fix algorithm settings
	//geneticAlgorithm.mutationRate = mMutationRate.get();
	//geneticAlgorithm.mutationAmount = mMutationAmount.get();
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

		//// change mating mode
		//if (mCrossoverSwitch.get())
		//	geneticAlgorithm.matingStrat = EMatingStrategy::SwitchSource;

		//if (mCrossoverInterpolate.get())
		//	geneticAlgorithm.matingStrat = EMatingStrategy::Interpolate;

		//if (mCrossoverGene.get())
		//	geneticAlgorithm.matingStrat = EMatingStrategy::Gene;

	}
	// TODO: add boolean params
	//geneticAlgorithm.groupGenes = mGroupGenes.get();
}

//--------------------------------------------------------------
void ArchitectureState::gotoNextStep()
{
	// TODO: check if one solution has been accepted
	if (currentStep == EEvolutionStep::Exterior)
	{
		if (selectedIndices.size() == 1)
		{
			int tile = selectedIndices[0];
			ofPolyline floor = exteriorEvolver.getBuildingAt(tile)->floorShapes[0];

			interiorEvolver.setFloorShape(floor);
			pCurrentEvolver = &interiorEvolver;
			currentStep = EEvolutionStep::Interior;

			selectedIndices.clear();
			pCurrentEvolver->generate(selectedIndices);
		}
	}
}
