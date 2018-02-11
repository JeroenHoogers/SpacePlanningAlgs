#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetVerticalSync(false);

	// use depth information for occlusion
	ofEnableDepthTest();

	// use normalized tex coords
	ofEnableNormalizedTexCoords();

	ofSetLineWidth(2);

	camera.setDistance(30);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackgroundGradient(ofColor(150, 150, 150), ofColor(100, 100, 100));

	camera.begin();

	ofEnableDepthTest();
	ofSetColor(255);
	ofPushMatrix();
	{
		ofTranslate(0, 2);
	//	ofNoFill();
		ofDrawBox(4);
	}
	ofPopMatrix();

	ofPushMatrix();
	{
		ofRotateZ(90);
		ofSetColor(50);
		ofDrawGridPlane(1);
	}
	ofPopMatrix();

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
		ofTranslate(10, 10);
		ofDrawBitmapStringHighlight("fps:" + ofToString(ofFpsCounter().getFps()), ofPoint());
	}
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
