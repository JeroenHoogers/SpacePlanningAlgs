#pragma once


class GeneticVariable
{
protected:
	int size;
	int index;

public:
	virtual void Parse() = 0;

};

//--------------------------------------------------------------
class IndoorSplit : public GeneticVariable
{
public:
	float position;
	int angle;

	IndoorSplit()
	{
		size = 2;
	}

	void Parse()
	{

	};
};

//--------------------------------------------------------------
class Extrusion : public GeneticVariable
{
public:
	float position;
	int floor;
	float amount;
	float angle;

	Extrusion()
	{
		size = 4;
	}

	void Parse()
	{
		//// TODO: make sure this is not close to any walls, maybe align to a grid?
		//position = gt[index]; // interpolation along shape
		//						// TODO: apply to multiple but not all floors?
		//floor = (gt[index + 1] > 0.5f) ? fminf(floorf((gt[index + 1] - 0.5f) * 2.0f * floors), floors - 1) : -1;

		//amount = ofLerp(-maxExtrusion, maxExtrusion, gt[index + 2]);
		//amount = (amount < 0) ? ofClamp(amount, -maxExtrusion, -minExtrusion) : ofClamp(amount, minExtrusion, maxExtrusion);

		//// calculate angle
		//float angle = 0;
		//if (gt[index + 3] < 0.2f)
		//	angle = ofLerp(-45.0f, 0, gt[i + 3] * 4);
		//if (gt[index + 3] > 0.8f)
		//	angle = ofLerp(0, 45.0f, (gt[i + 3] - 0.75f) * 4);
	};

};

