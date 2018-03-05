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

	camera.setDistance(30);
	camera.disableRoll();

	//post.init(ofGetWidth(), ofGetHeight());
	//post.createPass<FxaaPass>();
	////post.createPass<BloomPass>();
	//post.createPass<SSAOPass>();
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
			ofTranslate(x, y);
			//ofScale(1.0f / tilew, 1.0f / tileh);

			//ofRectangle rect = ofRectangle(0, 0, tilew, tileh);
			ofRectangle rect = ofRectangle(x, y, tilew, tileh);
			
			drawTile(rect);
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
}


void ArchitectureState::drawTile(ofRectangle viewport)
{
	ofSetLineWidth(1.5);
	ofEnableDepthTest();

	//post.begin(camera);
	light.setPosition(-camera.getGlobalPosition());
	light.lookAt(ofVec3f(0, 0, 0));

	camera.begin(viewport);

	// draw outlines
	ofNoFill();
	ofSetColor(140);

	ofPushMatrix();
	{
		ofTranslate(0, 6);
		ofDrawBox(16, 12, 16);
	}
	ofPopMatrix();

	ofPushMatrix();
	{
		ofTranslate(12, 3);
		ofDrawBox(8, 6, 12);
	}
	ofPopMatrix();

	ofEnableLighting();
	//phong.begin();

	// draw shapes
	ofSetColor(255);
	ofFill();

	ofPushMatrix();
	{
		ofTranslate(0, 6);
		ofDrawBox(16, 12, 16);
	}
	ofPopMatrix();

	ofPushMatrix();
	{
		ofTranslate(12, 3);
		ofDrawBox(8, 6, 12);
	}
	ofPopMatrix();

	ofDisableLighting();
	//phong.end();

	ofPushMatrix();
	{
		//ofRotateX(-90);
		ofRotateZ(90);

		ofSetColor(120);
		//ofDrawPlane(0, 0, 50, 50);
		ofDrawGridPlane(2, 16);
	}
	ofPopMatrix();


	ofDisableDepthTest();

	//ofDrawAxis(5);
	//post.end();

	camera.end();
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
}

//--------------------------------------------------------------
void ArchitectureState::mousePressed(int x, int y, int button)
{

}