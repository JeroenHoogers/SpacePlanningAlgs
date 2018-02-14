#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetVerticalSync(false);

	// use depth information for occlusion
	ofEnableDepthTest();

	// use normalized tex coords
	ofEnableNormalizedTexCoords();

	//glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);
	// use AA
	ofEnableAntiAliasing();

	ofSetCoordHandedness(ofHandednessType::OF_RIGHT_HANDED);

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
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackgroundGradient(ofColor(200, 200, 200), ofColor(150, 150, 150));

	ofEnableLighting();
	ofEnableDepthTest();

	//post.begin(camera);

	light.setPosition(-camera.getGlobalPosition());
	light.lookAt(ofVec3f(0, 0, 0));

	camera.begin();

	// draw outlines
	ofNoFill();
	ofSetColor(50);


	ofPushMatrix();
	{
		ofTranslate(0, 4);
		ofDrawBox(8, 8, 12);

		ofTranslate(0, 7);
		ofDrawBox(6, 6, 6);

	}
	ofPopMatrix();


	//phong.begin();

	// draw shapes
	ofSetColor(255);
	ofFill();

	ofPushMatrix();
	{
		ofTranslate(0, 4);
		ofDrawBox(8, 8, 12);

		ofTranslate(0, 7);
		ofDrawBox(6, 6, 6);
	}
	ofPopMatrix();

	ofDisableLighting();

	//phong.end();


	//ofPushMatrix();
	//{
	//	ofRotateX(90);
	//	ofSetColor(255);
	//	ofFill();
	//	ofDrawPlane(20, 20);

	//}
	//ofPopMatrix();

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

	drawGui();
}

//--------------------------------------------------------------
void ofApp::drawGui()
{
	ofSetColor(255);

	ofPushMatrix();
	{
		ofTranslate(10, 20);
		ofDrawBitmapStringHighlight("fps: " + ofToString(ofGetFrameRate()), ofPoint());
	}
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
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
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	post.init(w, h);
	//post.createPass<FxaaPass>();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
