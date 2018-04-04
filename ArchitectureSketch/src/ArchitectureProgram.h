#pragma once

#include "ofMain.h"

struct Room
{
	string name;
	float area;
	float min;
	float max;

	Room(string _name, float _area)
	{
		name = _name;
		area = _area;

		// calculate from area using 2:1 ratio rule
		min = sqrt(_area / 2.0f);
		max = min * 2;
	}

	Room(string _name, float _area, float _min, float _max)
	{
		name = _name;
		area = _area;
		min = _min;
		max = _max;
	}
};

class ArchitectureProgram
{
public:
	bool terracedLeft = false;
	bool terracedRight = false;

	int lotWidth = 25;
	int lotDepth = 25;

	int inhabitants = 4;
	int stories = 2;

	ofRectangle getLotRectangle()
	{
		return ofRectangle(-lotWidth * 0.5f, -lotDepth * 0.5f, lotWidth, lotDepth);
	};

	vector<Room> rooms;
	
	ArchitectureProgram()
	{
		// add test rooms
		rooms.push_back(Room("Mstr. Bedroom", 20.0f));
		rooms.push_back(Room("Bathroom", 3.75f));
		rooms.push_back(Room("Living", 30.0f));
		rooms.push_back(Room("Kitchen", 9.0f));
		rooms.push_back(Room("Bedroom", 13.0f));
	}

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