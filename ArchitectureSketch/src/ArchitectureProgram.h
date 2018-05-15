#pragma once

#include "ofMain.h"

struct Room
{
	string name;
	float area;
	float min;
	float max;
	string code;

	Room(string _name, string _code, float _area)
	{
		name = _name;
		code = _code;
		area = _area;

		// calculate from area using 2:1 ratio rule
		min = sqrt(_area / 2.0f);
		max = min * 2;
	}

	Room(string _name, string _code, float _area, float _min, float _max)
	{
		name = _name;
		code = _code;
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

	int lotWidth = 15;
	int lotDepth = 15;

	int inhabitants = 4;
	int stories = 1;

	ofRectangle getLotRectangle()
	{
		return ofRectangle(-lotWidth * 0.5f, -lotDepth * 0.5f, lotWidth, lotDepth);
	};

	vector<Room> rooms;
	
	ArchitectureProgram()
	{
		// add test rooms
		rooms.push_back(Room("Mstr. Bedroom", "M. Be", 20.0f));
		rooms.push_back(Room("Bathroom", "Ba", 3.75f));
		rooms.push_back(Room("Living", "Li", 30.0f));
		rooms.push_back(Room("Kitchen", "Ki", 9.0f));
		rooms.push_back(Room("Bedroom", "Be", 13.0f));
	}

	float getRoomAreaRatio(int room)
	{
		if (rooms.size() >= room || room < 0)
			return 0;

		// calculate total area
		float total = 0;
		for (int i = 0; i < rooms.size(); i++)
		{
			total += rooms[i].area;
		}

		// calculate fraction
		return total / rooms[room].area;
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