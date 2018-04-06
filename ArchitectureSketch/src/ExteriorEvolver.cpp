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

	// TODO: make mutation rate and amount variable
	geneticAlgorithm.setup(1000, 19, 0.2f, 0.4f);

	buildings.clear();

	for (int i = 0; i < geneticAlgorithm.population.size(); i++)
	{
		Building building = Building();
		buildings.push_back(building);
	}

	candidates.clear();

	// evaluate all candidates
	generate(vector<int>());
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
		}
	}

	candidates.clear();

	if (selection.size() > 0)
	{
		// let the genetic algorithm generate offspring based on the selection
		geneticAlgorithm.generateOffspring();
	}
	else
	{
		geneticAlgorithm.generateRandomPopulation();
		//setupEvolution();
	}

	// generate phenotypes for the new population
	for (int i = 0; i < geneticAlgorithm.population.size(); i++)
	{
		//for (int j = 0; j < 100; j++)
		//{
		buildings[i].LoadFromGenotype(geneticAlgorithm.population[i], *pProgram);

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