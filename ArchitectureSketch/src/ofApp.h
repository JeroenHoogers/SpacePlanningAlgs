#pragma once

#include "ofMain.h"
#include "ofxStateMachine.h"
#include "SharedData.h"
#include "EvolutionState.h"
#include "ProjectState.h"
#include "ArchitectureState.h"
#include "ProgramState.h"
#include "DebugState.h"
#include "EditState.h"

class ofApp : public ofBaseApp
{
	private:
	itg::ofxStateMachine<SharedData> * mStateMachine;

	public:
		void setup();
		void setupGui();

		void update();
		void draw();

		void drawGui();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		ofLight light;
		ofxEasyCam camera;

		ofShader phong;

		ofxPanel guiPanel;


};
