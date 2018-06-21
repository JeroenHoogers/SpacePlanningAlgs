#include "ProjectState.h"

//--------------------------------------------------------------
ProjectState::ProjectState()
{
}

//--------------------------------------------------------------
ProjectState::~ProjectState()
{
}

//--------------------------------------------------------------
void ProjectState::stateEnter()
{
	building = getSharedData().building;

	// if the building has an interior, show it, otherwise show the entire building
	if (building.hasInterior)
		mVisibleFloor = 0;
	else
		mVisibleFloor = -1;

	//// Update lot polygon
	//lotPolygon.clear();

	//lotPolygon.addVertex(
	//	ofPoint(-pProgram->lotWidth * 0.5f, 0, -pProgram->lotDepth * 0.5f));
	//lotPolygon.addVertex(
	//	ofPoint(-pProgram->lotWidth * 0.5f, 0, pProgram->lotDepth * 0.5f));
	//lotPolygon.addVertex(
	//	ofPoint(pProgram->lotWidth * 0.5f, 0, pProgram->lotDepth * 0.5f));
	//lotPolygon.addVertex(
	//	ofPoint(pProgram->lotWidth * 0.5f, 0, -pProgram->lotDepth * 0.5f));
	//lotPolygon.close();

	light.enable();

	mVisibleFloor.setMax(pProgram->stories - 1);
}

//--------------------------------------------------------------
void ProjectState::stateExit()
{
	light.disable();
}

//--------------------------------------------------------------
void ProjectState::setup()
{
	// use AA
	ofEnableAntiAliasing();

	// use normalized tex coords
	ofEnableNormalizedTexCoords();

	//glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);

	// load shader
	//phong.load("shaders/phong.vert", "shaders/phong.frag");

	// setup light
	light.setDirectional();
	light.setAmbientColor(ofColor(60, 60, 80));
	light.setDiffuseColor(ofColor(210, 200, 180));

	light.setPosition(-150, -200, -400);
	light.lookAt(ofVec3f(0, 0, 0));
	//light.enable();

	ofSetLineWidth(1.5);

	camera.setDistance(40);
	camera.disableRoll();

	pProgram = &getSharedData().program;

	geneticAlgorithm.setup(1, 12);

	// back button
	mBackButton.setup("Back");
	mBackButton.setPosition(ofPoint(20, 75));
	mBackButton.addListener(this, &ProjectState::backButtonPressed);

	// indoor 1 button
	mInterior1Button.setup("Create Interior 1 (Split Tree)", 290);
	mInterior1Button.setPosition(ofPoint(20, 20));
	mInterior1Button.addListener(this, &ProjectState::interior1ButtonPressed);

	// indoor 2 button
	mInterior2Button.setup("Create Interior 2 (BFS)", 290);
	mInterior2Button.setPosition(ofPoint(20, 45));
	mInterior2Button.addListener(this, &ProjectState::interior2ButtonPressed);

	mFloorDownButton.setup("<", 30);
	mFloorDownButton.setPosition(ofPoint(80, 150));
	mFloorDownButton.addListener(this, &ProjectState::floorDownButtonPressed);

	mFloorUpButton.setup(">", 50);
	mFloorUpButton.setPosition(ofPoint(110, 150));
	mFloorUpButton.addListener(this, &ProjectState::floorUpButtonPressed);



	// TODO: replace with shared data
	//building.LoadFromGenotype(geneticAlgorithm.generateRandomDna());
	//building.GenerateBuilding();
}

//--------------------------------------------------------------
void ProjectState::update()
{

}

//--------------------------------------------------------------
void ProjectState::backButtonPressed()
{
	// go back to architecture state
	changeState(ArchitectureState_StateName);
}

//--------------------------------------------------------------
void ProjectState::interior1ButtonPressed()
{
	// signal for split tree approach
	getSharedData().switchToSplitTree = true;

	// go to architecture state
	changeState(ArchitectureState_StateName);
}

//--------------------------------------------------------------
void ProjectState::interior2ButtonPressed()
{
	// signal for bfs based approach
	getSharedData().switchToBFS = true;

	// go to architecture state
	changeState(ArchitectureState_StateName);
}


//--------------------------------------------------------------
void ProjectState::floorUpButtonPressed()
{
	if (mVisibleFloor < pProgram->stories - 1)
		mVisibleFloor++;
}

//--------------------------------------------------------------
void ProjectState::floorDownButtonPressed()
{
	if (mVisibleFloor > -1)
		mVisibleFloor--;
}

//--------------------------------------------------------------
void ProjectState::gotoNextStep()
{
	//// set variables
	//changeState(ArchitectureState_StateName);

	//// restart
	//changeState(ProgramState_StateName);
}

//--------------------------------------------------------------
void ProjectState::draw()
{
	// use depth information for occlusion
	ofEnableDepthTest();

	light.setPosition(-camera.getGlobalPosition() + ofVec3f(0, 2, 0));
	light.lookAt(ofVec3f(0, 0, 0));

	ofBackground(ofColor(200));

	ofSetLineWidth(1.5);
	ofEnableDepthTest();

	//post.begin(camera);

	camera.begin();

	// draw outlines
	ofNoFill();
	//ofSetColor(140);

	// draw outlines
	if (drawOutlines)
	{
		ofNoFill();
		ofSetColor(140);

		//ofPushMatrix();
		//{
		//	ofTranslate(0, 6);
		//	ofDrawBox(16, 12, 16);
		//}
		//ofPopMatrix();

		//ofPushMatrix();
		//{
		//	ofTranslate(12, 3);
		//	ofDrawBox(8, 6, 12);
		//}
		//ofPopMatrix();
	}
	ofEnableLighting();
	//phong.begin();

	// draw shapes
	ofSetColor(255);
	ofFill();

	building.draw(mVisibleFloor);

	// draw scale models
	ofSetColor(30);
	ofRectangle boundingRect = building.GetFloorBoundingBox(0);

	ofDrawCylinder(boundingRect.getMinX() - 1.0f, 0.9f, 0, 0.2f, 1.8f);
	ofDrawCylinder(boundingRect.getMaxX() + 1.0f, 0.9f, 0, 0.2f, 1.8f);
	ofDrawCylinder(0, 0.9f, boundingRect.getMinY() - 1.0f, 0.2f, 1.8f);
	ofDrawCylinder(0, 0.9f, boundingRect.getMaxY() + 1.0f, 0.2f, 1.8f);

	ofDisableLighting();

	// HACK: hack depth buffer range to make sure the lines render on top of the geometry
	glDepthRange(0.0, 0.9995);

	//// draw lot
	//ofSetColor(80);
	//ofSetLineWidth(2.0f);
	//ofNoFill();
	//lotPolygon.draw();

	//// HACK: hack depth buffer range to make sure the lines render on top of the geometry
	//glDepthRange(0.0005, 1.0);

	//phong.end();

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

	ofDisableDepthTest();
	//ofDrawAxis(5);
	//post.end();

	//camera.begin();

	ofDisableLighting();
	camera.end();

	drawGUI();
}

//--------------------------------------------------------------
void ProjectState::drawGUI()
{
	ofSetColor(255);

	mBackButton.draw();
	mInterior1Button.draw();
	mInterior2Button.draw();

	ofDrawBitmapStringHighlight("Floor:", ofPoint(20, 162));
	mFloorUpButton.draw();
	mFloorDownButton.draw();


	//string controls = "Controls: \n(o): toggle orthographic\n(t): top view \n(f): front view \n(s): side view";
	//ofDrawBitmapStringHighlight(controls, ofPoint(10, 20));

	//ofTranslate(10, ofGetWidth() - 140);

	ofDrawBitmapStringHighlight("fps: " + ofToString(ofGetFrameRate()), ofPoint(ofGetWidth() - 110, 20));
}


//--------------------------------------------------------------
string ProjectState::getName()
{
	return ProjectState_StateName;
}

//--------------------------------------------------------------
void ProjectState::keyPressed(int key)
{
	float dist = camera.getDistance();
	if (key == 't')
	{
		camera.setPosition(0, dist, 0);
		camera.lookAt(ofPoint(0, 0, 0));
		//camera.setScale(0.5);

	}
	if (key == 'f')
	{
		camera.setPosition(0, 0, dist);
		camera.lookAt(ofPoint(0, 0, 0));
	}
	if (key == 's')
	{
		camera.setPosition(dist, 0, 0);
		camera.lookAt(ofPoint(0, 0, 0));
	}

	if (key == 'o')
	{
		if (!camera.getOrtho())
		{
			camera.setScale(0.05, 0.05, 0.05);
			camera.enableOrtho();
		}
		else
		{
			//camera.setScale(0.1, 0.1, 0.1);
			camera.disableOrtho();
		}
	}

	if (key == 'l')
	{
		drawOutlines = !drawOutlines;
	}

	if (key == 'r')
	{
		// refresh building
		//building.LoadFromGenotype(geneticAlgorithm.generateRandomDna());
		//building.GenerateBuilding();
	}

	// visible floor down
	if (key == '[')
	{
		if (mVisibleFloor > -1)
			mVisibleFloor--;
	}

	// visible floor up
	if (key == ']')
	{
		if (mVisibleFloor < pProgram->stories - 1)
			mVisibleFloor++;
	}
}

//--------------------------------------------------------------
void ProjectState::mousePressed(int x, int y, int button)
{

}