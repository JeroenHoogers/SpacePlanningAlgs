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

}

//--------------------------------------------------------------
void ProjectState::stateExit()
{

}

//--------------------------------------------------------------
void ProjectState::setup()
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
	light.setAmbientColor(ofColor(65, 65, 80));
	light.setDiffuseColor(ofColor(220, 210, 180));

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
void ProjectState::update()
{

}

//--------------------------------------------------------------
void ProjectState::draw()
{
	ofSetLineWidth(1.5);
	ofBackgroundGradient(ofColor(200, 200, 200), ofColor(125, 125, 125));

	ofEnableDepthTest();

	//post.begin(camera);

	light.setPosition(-camera.getGlobalPosition());
	light.lookAt(ofVec3f(0, 0, 0));

	camera.begin();

	// draw outlines
	if (drawOutlines)
	{
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
	}
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
		ofRotateZ(90);

		ofSetColor(120);
		ofDrawGridPlane(2, 16);
	}
	ofPopMatrix();


	ofDisableDepthTest();


	ofDrawAxis(5);
	//post.end();

	//camera.begin();
	camera.end();

	drawGUI();
}

//--------------------------------------------------------------
void ProjectState::drawGUI()
{
	ofSetColor(255);

	string controls = "Controls: \n(o): toggle orthographic\n(t): top view \n(f): front view \n(s): side view";
	ofDrawBitmapStringHighlight(controls, ofPoint(10, 20));

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
}

//--------------------------------------------------------------
void ProjectState::mousePressed(int x, int y, int button)
{

}