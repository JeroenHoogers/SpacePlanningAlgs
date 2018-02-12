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
	light.setDiffuseColor(ofColor(220, 210, 150));
	light.setOrientation(ofPoint(4, 1, -6));

	ofSetLineWidth(1);

	camera.setDistance(30);
	camera.disableRoll();

}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackgroundGradient(ofColor(150, 150, 150), ofColor(100, 100, 100));

	camera.begin();
	ofEnableLighting();

	ofEnableDepthTest();
	ofSetColor(255);
	ofPushMatrix();
	{
		ofTranslate(0, 4);
		ofFill();
		ofDrawBox(8);

		// outline pass
		ofNoFill();
		ofSetColor(50);
		ofDrawBox(8);
	}
	ofPopMatrix();
	ofDisableLighting();

	ofPushMatrix();
	{
		ofRotateZ(90);
		ofSetColor(80);
		
		ofDrawGridPlane(1, 32);
	}
	ofPopMatrix();

	ofDisableDepthTest();

	//ofDrawAxis(5);

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
