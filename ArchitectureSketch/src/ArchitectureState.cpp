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

	testModel.m1 = Mass(-8, 8, -6, 6, 2);
	testModel.m2 = Mass(-16, -8, -5, 5, 1);
	testModel.m3 = Mass(8, 16, -5, 5, 1);

	// button events
	generateNextGenButton.addListener(this, &ArchitectureState::generateButtonPressed);

	//// major parameters (require genetic algorithm reset)
	//mDimensions.addListener(this, &ArchitectureState::majorParameterChanged);
	//mPopulationSize.addListener(this, &ArchitectureState::majorParameterChanged);

	// minor parameters (do not require GA reset)
	mMutationRate.addListener(this, &ArchitectureState::minorParameterChanged);
	mMutationAmount.addListener(this, &ArchitectureState::minorParameterChanged);

	// setup gui 
	gui.setup("GA Settings");
	//gui.add(mDimensions);
	//gui.add(mPopulationSize);
	gui.add(mMutationRate);
	gui.add(mMutationAmount);
	gui.add(generateNextGenButton.setup("Generate offspring"));

	setupEvolution();
}

//--------------------------------------------------------------
void ArchitectureState::setupEvolution()
{
	// setup genetic algorithm
	geneticAlgorithm.setup(9, 15, mMutationRate.get(), mMutationAmount.get());

	massModels.clear();

	for (int i = 0; i < geneticAlgorithm.population.size(); i++)
	{
		massModels.push_back(convertGenotype(geneticAlgorithm.population[i]));
	}
}

//--------------------------------------------------------------
MassModel ArchitectureState::convertGenotype(Genotype genotype)
{
	// range -30, 30
	float max = 25;
	int h = 3;
	MassModel mm;
	
	mm.m1 = Mass(
		(genotype[0] - 0.5f) * max,
		(genotype[1] - 0.5f) * max,
		(genotype[2] - 0.5f) * max,
		(genotype[3] - 0.5f) * max,
		floorf(genotype[4] * h + 0.99f)
	);

	mm.m2 = Mass(
		(genotype[5] - 0.5f) * max,
		(genotype[6] - 0.5f) * max,
		(genotype[7] - 0.5f) * max,
		(genotype[8] - 0.5f) * max,
		floorf(genotype[9] * h + 0.99f)
	);

	mm.m3 = Mass(
		(genotype[10] - 0.5f) * max,
		(genotype[11] - 0.5f) * max,
		(genotype[12] - 0.5f) * max,
		(genotype[13] - 0.5f) * max,
		floorf(genotype[14] * h + 0.99f)
	);

	return mm;
}

//--------------------------------------------------------------
void ArchitectureState::update()
{

}

//--------------------------------------------------------------
void ArchitectureState::draw()
{
	ofBackgroundGradient(ofColor(200, 200, 200), ofColor(125, 125, 125));

	int tilew = (ofGetWidth() / tilesHorizontal);
	int tileh = (ofGetHeight() / tilesVertical);

	for (int i = 0; i < (tilesHorizontal * tilesVertical); i++)
	{
		int x = (i % tilesHorizontal) * tilew;
		int y = (i / tilesVertical) * tileh;

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

	for (int i = 0; i < (tilesHorizontal * tilesVertical); i++)
	{
		// draw tiles
		int x = (i % tilesHorizontal) * tilew;
		int y = (i / tilesVertical) * tileh;

		ofDrawRectangle(x, y, 0, x + tilew, y + tileh);
	}
	//drawGUI();

	camera.begin();
	camera.end();

	// show generation
	ofSetColor(10);
	ofDrawBitmapStringHighlight("Generation: " + ofToString(geneticAlgorithm.currentGeneration), 15, 25);

	gui.draw();
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

	// draw mass 1
	//ofPushMatrix();
	//{
	ofPoint pos;
	ofPoint sz;

	// draw mass 1
	pos = massModels[index].m1.getPoint();
	sz = massModels[index].m1.getSize();

	ofSetColor(180, 180, 80, 255);
	ofDrawBox(pos, sz.x, sz.y, sz.z);

	// draw mass 2
	pos = massModels[index].m2.getPoint();
	sz = massModels[index].m2.getSize();

	ofSetColor(80, 80, 180, 255);
	ofDrawBox(pos, sz.x, sz.y, sz.z);

	// draw mass 3
	pos = massModels[index].m3.getPoint();
	sz = massModels[index].m3.getSize();

	ofSetColor(80, 180, 80, 255);
	ofDrawBox(pos, sz.x, sz.y, sz.z);


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

		ofSetColor(120);
		//ofDrawPlane(0, 0, 50, 50);
		ofDrawGridPlane(2, 8);
	}
	ofPopMatrix();

	ofDisableDepthTest();

	//ofDrawAxis(5);
	//post.end();

	camera.end();
}

//void ArchitectureState::drawTile(ofRectangle viewport)
//{
//	ofSetLineWidth(1.5);
//	ofEnableDepthTest();
//
//	//post.begin(camera);
//	light.setPosition(-camera.getGlobalPosition());
//	light.lookAt(ofVec3f(0, 0, 0));
//
//	camera.begin(viewport);
//
//	//// draw outlines
//	ofNoFill();
//	//ofSetColor(140);
//
//	ofPushMatrix();
//	{
//		ofSetColor(180 * 0.8f, 180 * 0.8f, 80 * 0.8f, 255);
//		ofTranslate(0, 6);
//		ofDrawBox(16, 12, 16);
//	}
//	ofPopMatrix();
//
//	ofPushMatrix();
//	{
//		ofSetColor(80 * 0.8f, 80 * 0.8f, 180 * 0.8f, 255);
//		ofTranslate(12, 3);
//		ofDrawBox(8, 6, 12);
//	}
//	ofPopMatrix();
//
//	ofPushMatrix();
//	{
//		ofSetColor(80 * 0.8f, 180 * 0.8f, 80 * 0.8f, 255);
//		ofTranslate(-12, 3);
//		ofDrawBox(8, 6, 12);
//	}
//	ofPopMatrix();
//
//	ofEnableLighting();
//	//phong.begin();
//
//	// draw shapes
//	ofSetColor(220);
//	ofFill();
//
//	ofPushMatrix();
//	{
//		ofSetColor(180, 180, 80, 255);
//		ofTranslate(0, 6);
//		ofDrawBox(16, 12, 16);
//	}
//	ofPopMatrix();
//
//	ofPushMatrix();
//	{
//		ofSetColor(80, 80, 180, 255);
//		ofTranslate(12, 3);
//		ofDrawBox(8, 6, 12);
//	}
//	ofPopMatrix();
//
//	ofPushMatrix();
//	{
//		ofSetColor(80, 180, 80, 255);
//		ofTranslate(-12, 3);
//		ofDrawBox(8, 6, 12);
//	}
//	ofPopMatrix();
//
//	ofDisableLighting();
//	//phong.end();
//
//	ofPushMatrix();
//	{
//		//ofRotateX(-90);
//		ofRotateZ(90);
//
//		ofSetColor(120);
//		//ofDrawPlane(0, 0, 50, 50);
//		ofDrawGridPlane(2, 16);
//	}
//	ofPopMatrix();
//
//
//	ofDisableDepthTest();
//
//	//ofDrawAxis(5);
//	//post.end();
//
//	camera.end();
//}


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
			camera.setScale(0.1, 0.1, 0.1);
			camera.enableOrtho();
		}
		else
		{
			//camera.setScale(0.1, 0.1, 0.1);
			camera.disableOrtho();
		}
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
		int tilew = (ofGetWidth() / tilesHorizontal);
		int tileh = (ofGetHeight() / tilesVertical);

		// calculate index based on mouse position
		int index = (x / tilew) + (y / tileh) * 3;
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

	massModels.clear();

	for (int i = 0; i < geneticAlgorithm.population.size(); i++)
	{
		massModels.push_back(convertGenotype(geneticAlgorithm.population[i]));
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