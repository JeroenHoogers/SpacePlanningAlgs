#pragma once

#include "ofMain.h"

class ArchitectureProgram
{
public:
	bool terracedLeft = false;
	bool terracedRight = false;

	int lotWidth = 25;
	int lotDepth = 25;

	int inhabitants = 2;
	int stories = 2;

	ofRectangle getLotRectangle()
	{
		return ofRectangle(-lotWidth * 0.5f, -lotDepth * 0.5f, lotWidth, lotDepth);
	};

//private:
//
//public:
//	bool livingroom = true;		
//	bool kitchen = true;
//	bool bathroom = true;
//	bool 
//	int bedrooms2person = 1;
//	int bedrooms1person = 1;
//	
	
};