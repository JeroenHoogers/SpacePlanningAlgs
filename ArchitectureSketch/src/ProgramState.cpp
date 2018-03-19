#include "ProgramState.h"

//--------------------------------------------------------------
ProgramState::ProgramState()
{
}

//--------------------------------------------------------------
ProgramState::~ProgramState()
{
}

//--------------------------------------------------------------
void ProgramState::stateEnter()
{

}

//--------------------------------------------------------------
void ProgramState::stateExit()
{
	updateProgram();
}

//--------------------------------------------------------------
void ProgramState::setup()
{
	mapRectangle = ofRectangle(520, 30, 400, 400);

	// setup gui
	mProgramParameters.setName("Program");
	mProgramParameters.add(mInhabitants);

	guiPanel.setup(mProgramParameters,"settings.xml",20,20);

	//guiPanel.add(mProgramParameters);
	mSiteParameters.setName("Site");
	mSiteParameters.add(mWidth);
	mSiteParameters.add(mDepth);

	mInhabitantsSlider.setPosition(ofPoint(20, 30));
	mStoriesSlider.setPosition(ofPoint(20, 60));

	mWidthSlider.setPosition(ofPoint(20, 200));
	mDepthSlider.setPosition(ofPoint(20, 230));

	mTerracedLeftToggle.setPosition(ofPoint(20, 260));
	mTerracedRightToggle.setPosition(ofPoint(20, 290));

	// accept button
	mAcceptButton.setup("Accept", 300, 20);
	mAcceptButton.setPosition(ofPoint(20, 350));

	mAcceptButton.addListener(this, &ProgramState::acceptButtonPressed);

	pProgram = &getSharedData().program;
}

//--------------------------------------------------------------
void ProgramState::update()
{

}

//--------------------------------------------------------------
void ProgramState::draw()
{
	//ofClear()
	ofSetLineWidth(1);
	ofBackground(200);
	//ofBackgroundGradient(ofColor(200, 200, 200), ofColor(125, 125, 125));

	ofSetColor(40);
	ofDrawBitmapString("Architectural Program", ofPoint(20, 20));

	//guiPanel.draw();
	
	mInhabitantsSlider.draw();
	mStoriesSlider.draw();

	//ofSetColor(255);
	ofDrawBitmapStringHighlight("Minimum area: " + ofToString(Measurements::getMinimumArea(mInhabitants.get(), mStories.get())) + " m2", 20, 100);


	ofSetColor(40);
	ofDrawBitmapString("Site constraints", ofPoint(20, 190));
	mWidthSlider.draw();
	mDepthSlider.draw();

	mTerracedLeftToggle.draw();
	mTerracedRightToggle.draw();

	mAcceptButton.draw();
	
	// draw the map
	drawSiteLayout();
}

//--------------------------------------------------------------
void ProgramState::drawSiteLayout()
{
	ofSetColor(40);
	ofDrawBitmapString("Site Layout", ofPoint(mapRectangle.getMinX(), 20));

	ofSetColor(160);
	ofFill();
	ofRect(mapRectangle);

	ofFill();
	ofPushMatrix();
	{
		ofTranslate(mapRectangle.getMinX(), mapRectangle.getMinY());

		ofSetColor(255);
		ofDrawBitmapString("Road", 180, 25);

		float x = 200 - mWidth.get() * 5.0f;
		float y = 200 - mDepth.get() * 5.0f;
		float w = mWidth.get() * 10.0f;
		float h = mDepth.get() * 10.0f;

		ofDrawArrow(ofVec2f(200, y - 30), ofVec2f(200, y - 8), 3);

		ofSetColor(190);



		ofSetLineWidth(2);
		ofDrawLine(0, y, 400, y);

		ofSetColor(255);
		ofRect(ofPoint(x, y), w, h);

		ofSetColor(40);
		ofDrawBitmapString(ofToString(mWidth.get() * mDepth.get()) + " m2", ofPoint(180, 200));

		ofSetColor(190);
		if(mTerracedLeft.get())
			ofRect(ofPoint(0, y), x, h);

		if (mTerracedRight.get())
			ofRect(ofPoint(x+w, y), x, h);
	}
	ofPopMatrix();

	ofSetColor(50);
	ofNoFill();
	ofRect(mapRectangle);
	// TODO: Draw width depth below the map

}

//--------------------------------------------------------------
string ProgramState::getName()
{
	return ProgramState_StateName;
}

//--------------------------------------------------------------
void ProgramState::keyPressed(int key)
{
	
}

//--------------------------------------------------------------
void ProgramState::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ProgramState::updateProgram()
{
	// inhabitants
	pProgram->inhabitants = mInhabitants.get();

	// lot dims
	pProgram->lotWidth = mWidth.get();
	pProgram->lotDepth = mDepth.get();
	pProgram->stories = mStories.get();

	// terraced 
	pProgram->terracedLeft = mTerracedLeft.get();
	pProgram->terracedRight = mTerracedRight.get();
}

//--------------------------------------------------------------
void ProgramState::acceptButtonPressed()
{
	// TODO: store program settings
	updateProgram();

	// change state to architecture
	changeState(ArchitectureState_StateName);


}