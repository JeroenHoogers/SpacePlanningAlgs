#include "ExteriorEvolver.h"


//--------------------------------------------------------------
ExteriorEvolver::ExteriorEvolver()
{
}

//--------------------------------------------------------------
ExteriorEvolver::~ExteriorEvolver()
{
}

//--------------------------------------------------------------
void ExteriorEvolver::setup(int _tiles, ArchitectureProgram* _pProgram)
{
	// base setup
	Evolver::setup(_tiles, _pProgram);

	int maxExtrusions = 6;
	int baseVars = 3;

	geneticAlgorithm.setup(1000, baseVars + maxExtrusions * 4, 0.25f, 0.4f);
	extrusionSelectionAlgorithm.setup(1000, maxExtrusions, 0.25f, 0.4f);

	buildings.clear();

	for (int i = 0; i < geneticAlgorithm.population.size(); i++)
	{
		Building building = Building();
		buildings.push_back(building);
	}

	candidates.clear();

	// evaluate all candidates
	generate(vector<int>());

	generation = 0;
}

//--------------------------------------------------------------
void ExteriorEvolver::generate(vector<int> selection)
{
	//// mark survivors from the last generation
	//for (int i = 0; i < mSelectionRectangles.size(); i++)
	//{
	//	if (i < nrSelected)
	//		mSelectionRectangles[i].prevSelected = true;
	//	else
	//		mSelectionRectangles[i].prevSelected = false;
	//}

	//massModels.clear();
	bool done = false;

	// find selected indices
	for (int i = 0; i < selection.size(); i++)
	{
		// convert the tile indices to the indices used by the genetic algorithm
		int tile = selection[i];

		if (tile < candidates.size())
		{
			int index = candidates[tile];

			// select the indices in the genetic algorithm
			geneticAlgorithm.select(index);
			extrusionSelectionAlgorithm.select(index);
		}
	}

	candidates.clear();

	if (selection.size() > 0)
	{
		// let the genetic algorithm generate offspring based on the selection
		geneticAlgorithm.generateOffspring();
		extrusionSelectionAlgorithm.generateOffspring();

		generation++;
	}
	else
	{
		geneticAlgorithm.generateRandomPopulation();
		extrusionSelectionAlgorithm.generateRandomPopulation();

		generation = 0;
		//setupEvolution();
	}

	// generate phenotypes for the new population
	for (int i = 0; i < geneticAlgorithm.population.size(); i++)
	{
		const vector<float>& buildingGene = geneticAlgorithm.population[i].genes;
		const vector<bool>& variableGene = extrusionSelectionAlgorithm.population[i].genes;


		// TODO: move building phenotype conversion to here
		float w = 5.0f + floorf(buildingGene[0] * (pProgram->lotWidth - 5.0f));
		float h = 5.0f + floorf(buildingGene[1] * (pProgram->lotDepth - 5.0f));

		//if (pProgram->terracedLeft && program.terracedRight)
		//	w = program.lotWidth;

		int maxFloors = 3;

		ofRectangle boundingBox = ofRectangle(
			-w * 0.5f, -h * 0.5f, w, h);

		// TODO: derive nr of floors from area
		//floors = fminf(floorf(gt[2] * maxFloors + 1.0f), maxFloors);
		int roofSelector = floorf(buildingGene[2] * 2.0f);
		ERoofType roofType = ERoofType::Flat;

		if (roofSelector != 0)
			roofType = ERoofType::Hip;

		float roofPitch = buildingGene[2] / 2.0f; // roof param

		int floors = pProgram->stories;
	
		vector<Extrusion> extrusions;

		float minExtrusion = 1.0f;
		float maxExtrusion = 1.0f + 3.0f;

		// create extrusions
		
		//for (size_t i = 3; i < gt.size() - 3; i += 4)
		//{
		int offset = 3;
		for(int i = 0; i < variableGene.size(); i++)
		{
			// is this extrusion is enabled?
			if (variableGene[i])
			{
				int j = offset + i * 4;
				// TODO: tweak this ratio
				// TODO: make sure this is not close to any walls, maybe align to a grid?
				float position = buildingGene[j]; // interpolation along shape
										// TODO: apply to multiple but not all floors?

				// which floor does this extrusion apply to?
				int floor = (buildingGene[j + 1] > 0.5f) ? fminf(floorf((buildingGene[j + 1] - 0.5f) * 2.0f * floors), floors - 1) : -1;

				float amount = ofLerp(-maxExtrusion, maxExtrusion, buildingGene[j + 2]);
				amount = (amount < 0) ? ofClamp(amount, -maxExtrusion, -minExtrusion) : ofClamp(amount, minExtrusion, maxExtrusion);

				// calculate angle
				float order = buildingGene[j + 3];
				//if (gt[i + 3] < 0.1f)
				//	angle = ofLerp(-45.0f, 0, gt[i + 3] * 4);
				//if (gt[i + 3] > 0.9f)
				//	angle = ofLerp(0, 45.0f, (gt[i + 3] - 0.75f) * 4);


				// create extrusion 
				extrusions.push_back(
					Extrusion(position, amount, floor, order));
			}
		}

		buildings[i].Create(w, h, floors, extrusions, roofType, roofPitch);


		//buildings[i].LoadFromGenotype(geneticAlgorithm.population[i].genes, *pProgram);

		// filter candidates
		if (isBuildingValid(buildings[i]))
		{
			candidates.push_back(i);
			if (candidates.size() <= tiles)
			{
				buildings[i].GenerateBuilding();
			}
		}
	}
}

//--------------------------------------------------------------
Building* ExteriorEvolver::getBuildingAt(int tile)
{
	if (tile < candidates.size())
	{
		int index = candidates[tile];

		// select the indices in the genetic algorithm
		return &buildings[index];
	}

	return NULL;
}

//--------------------------------------------------------------
bool ExteriorEvolver::isBuildingValid(Building& building)
{
	// get minimum area
	int stories = pProgram->stories;
	int inhabitants = pProgram->inhabitants;

	float minimumArea = Measurements::getMinimumArea(inhabitants, stories);

	// area check
	if (building.GetTotalArea() < minimumArea)
		return false;

	ofRectangle lotRect = pProgram->getLotRectangle();

	for (size_t i = 0; i < building.floorShapes.size(); i++)
	{
		ofRectangle bb = building.floorShapes[i].getBoundingBox();
		ofRectangle intersection = lotRect.getIntersection(bb);

		// check whether the mass is inside the bounds of the plot
		if (intersection != bb)
			return false;

		// minimum line segment distance check
		for (size_t j = 0; j < building.floorShapes[i].size(); j++)
		{
			// TODO: Check if correct
			// filter odd angles
			float angle = building.floorShapes[i].getAngleAtIndex(j);
			if (angle < 45.0f || angle > 135.0f)
				return false;

			// check minimum wall length
			ofPoint p = building.floorShapes[i][j];
			ofPoint q = building.floorShapes[i][(j + 1) % building.floorShapes[i].size()];
			float dist = p.distance(q);

			if (dist > 0.0f && dist < 1.0f)
				return false;
		}

		// check for invalid (self-intersecting) geometry
		if (MeshHelper::hasSelfIntersections(building.floorShapes[i]))
			return false;
	}

	return true;
}