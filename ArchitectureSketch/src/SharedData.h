#pragma once

#include "ArchitectureProgram.h"
#include "Building.h"


#define ProjectState_StateName "ProjectState"
#define DebugState_StateName "DebugState"
#define EditState_StateName "EditState"
#define ArchitectureState_StateName "ArchitectureState"

class SharedData 
{
public:
	ArchitectureProgram program;

	Building building;

	ofPolyline floorshape;

	// TODO: make this a proper enum
	bool switchToExterior = false;
	bool switchToSplitTree = false;
	bool switchToBFS = false;
};