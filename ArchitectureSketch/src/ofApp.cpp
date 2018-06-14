#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetVerticalSync(false);

	ofSetCoordHandedness(ofHandednessType::OF_RIGHT_HANDED);
	ofEnableAntiAliasing();

	mStateMachine = new itg::ofxStateMachine<SharedData>();
	mStateMachine->addState(new EvolutionState());
	mStateMachine->addState(new ProgramState());
	mStateMachine->addState(new ArchitectureState());
	mStateMachine->addState(new DebugState());
	//mStateMachine->addState(new EditState());
	mStateMachine->addState(new ProjectState());

	mStateMachine->changeState(ProgramState_StateName);
}

//--------------------------------------------------------------
void ofApp::setupGui()
{
	//guiPanel = ofxPanel();

	//guiPanel.add()
}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{
	
}

//--------------------------------------------------------------
void ofApp::drawGui()
{
	ofSetColor(255);

	ofPushMatrix();
	{
		ofTranslate(10, 20);
		string controls = "Controls: \n(o) Toggle Othographic camera\n(t): top view \n(f): Front view";
		ofDrawBitmapStringHighlight("fps: " + ofToString(ofGetFrameRate()) + "\n" + controls, ofPoint());
	}
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

	if (key == 'e')
	{
		mStateMachine->changeState(EvolutionState_StateName);
	}

	if (key == 'p')
	{
		mStateMachine->changeState(ProgramState_StateName);
	}

	if (key == 'a')
	{
		mStateMachine->changeState(ArchitectureState_StateName);
	}

	if (key == 'd')
	{
		mStateMachine->changeState(DebugState_StateName);
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
	//post.init(w, h);
	//post.createPass<FxaaPass>();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
