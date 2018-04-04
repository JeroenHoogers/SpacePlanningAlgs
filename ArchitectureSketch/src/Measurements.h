#pragma once
#include "ofMain.h"

class Measurements
{
private:
	//map<Room, float>

public:
	// floor height
	float floorHeight = 3.0f;

	float ceilingHeight = 0.3f;
	float wallWidth = 0.2f;

	static float getMinimumArea(int inhabitants, int stories)
	{
		// From the metric planning handbook

		float targetArea = 0.0f;
		switch (inhabitants)
		{
		case 1:
			targetArea = 33.0f;

			if (stories > 1)
				targetArea = -1;
			break;
		case 2:
			targetArea = 48.5f;

			if (stories > 1)
				targetArea = -1;
			break;
		case 3:
			targetArea = 61.0f;

			if (stories > 1)
				targetArea = -1;
			break;
		case 4:
			if (stories == 1)
				targetArea = 71.5;
			else if (stories == 2)
				targetArea = 76.5f;
			else
				targetArea = -1;

			break;
		case 5:

			if (stories == 1)
				targetArea = 71.5;
			else if (stories == 2)
				targetArea = 86.5f;
			else
				targetArea = 98.5f;
			break;
		case 6:
			if (stories == 1)
				targetArea = 88.5f;
			else if (stories == 2)
				targetArea = 97.0f;
			else
				targetArea = 102.0f;

			break;
		case 7:
			if (stories == 2)
				targetArea = 114.5f;
			else if (stories == 3)
				targetArea = 118.5f;
			else
				targetArea = -1;
			break;
		default:
			break;
		}

		return targetArea;
	}
};