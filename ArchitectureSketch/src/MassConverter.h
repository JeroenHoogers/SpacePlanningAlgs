#pragma once

#include "Building.h"
#include "PhenotypeConverter.h"
#include "ArchitectureProgram.h"
#include "MeshHelper.h"
#include "Measurements.h"

class MassConverter : public PhenotypeConverter
{
public:
	MassConverter();
	~MassConverter();

	bool isValid(Genotype gt);

private:
	Building convert(Genotype gt);
};

MassConverter::MassConverter()
{

}

MassConverter::~MassConverter()
{
}

//--------------------------------------------------------------
Building MassConverter::convert(Genotype gt)
{
	//Building building;
	////vector<ofPolyline> floorShapes;

	//float w = 11.0f + floorf(gt[0] * 8.0f) * 2;
	//float h = 11.0f + floorf(gt[1] * 8.0f) * 2;

	//int floors = pProgram->stories;

	//boundingBox = ofRectangle(
	//	-w * 0.5f, -h * 0.5f, w, h);

	//// TODO: derive nr of floors from area
	////floors = fminf(floorf(gt[2] * maxFloors + 1.0f), maxFloors);

	//// TODO: separate subdivs and extrusions

	////subdivs.clear();
	//extrusions.clear();

	//float minExtrusion = 1.0f;
	//float maxExtrusion = 5.0f;

	//for (size_t i = 2; i < gt.size() - 3; i += 4)
	//{
	//	// which floor does this extrusion apply to?
	//	// TODO: tweak this ratio

	//	// TODO: make sure this is not close to any walls, maybe align to a grid?
	//	float position = gt[i]; // interpolation along shape
	//							// TODO: apply to multiple but not all floors?
	//	int floor = (gt[i + 1] > 0.5f) ? fminf(floorf((gt[i + 1] - 0.5f) * 2.0f * floors), floors - 1) : -1;

	//	float amount = ofLerp(-maxExtrusion, maxExtrusion, gt[i + 2]);
	//	amount = (amount < 0) ? ofClamp(amount, -maxExtrusion, -minExtrusion) : ofClamp(amount, minExtrusion, maxExtrusion);

	//	// calculate angle
	//	float angle = 0;
	//	if (gt[i + 3] < 0.25f)
	//		angle = ofLerp(-45.0f, 0, gt[i + 3] * 4);
	//	if (gt[i + 3] > 0.75f)
	//		angle = ofLerp(0, 45.0f, (gt[i + 3] - 0.75f) * 4);

	//	// create subdivs
	//	//subdivs.push_back(Subdivision(gt[i]));

	//	// add extrusion
	//	extrusions.push_back(
	//		Extrusion(position, amount, floor, angle));
	//}
}

//--------------------------------------------------------------
bool MassConverter::isValid(Genotype genotype)
{
	Building building = convert(genotype);

	int stories = pProgram->stories;
	int inhabitants = pProgram->inhabitants;

	float minimumArea = Measurements::getMinimumArea(inhabitants, stories);

	// area check
	if (building.GetTotalArea() < minimumArea)
		return false;

	// check whether the mass is inside the bounds of the plot

	// check for invalid geometry
	for (size_t i = 0; i < building.floorShapes.size(); i++)
	{
		if (MeshHelper::hasSelfIntersections(building.floorShapes[i]))
			return false;
	}

	return true;
}

