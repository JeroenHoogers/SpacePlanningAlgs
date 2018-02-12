#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetVerticalSync(false);

	// use depth information for occlusion
	ofEnableDepthTest();

	// use normalized tex coords
	ofEnableNormalizedTexCoords();

	light.enable();
	light.setDirectional();
	light.setAmbientColor(ofColor(75, 75, 90));
	light.setDiffuseColor(ofColor(220, 210, 180));
	light.setGlobalOrientation(ofQuaternion(55, ofVec3f(1,0.4,0.4)));

	ofSetLineWidth(2);

	camera.setDistance(30);
	camera.disableRoll();

	post.init(ofGetWidth(), ofGetHeight());
	post.createPass<FxaaPass>();
	post.createPass<SSAOPass>();

}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackgroundGradient(ofColor(150, 150, 150), ofColor(100, 100, 100));

	ofEnableLighting();
	ofEnableDepthTest();

	//post.begin(camera);
	camera.begin();

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
		
		ofSetColor(80);
		ofDrawGridPlane(2, 16);
	}
	ofPopMatrix();

	//post.end();


	ofDisableDepthTest();


	ofDrawAxis(5);

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
		//camera.enableOrtho();
		//camera.setScale(0.5);

	}
	if (key == 'f')
	{
		camera.setPosition(dist, 0, 0);
		camera.lookAt(ofPoint(0, 0, 0));
	//	camera.enableOrtho();

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

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
