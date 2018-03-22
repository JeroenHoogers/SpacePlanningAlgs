#include "StraightSkeleton.h"


//--------------------------------------------------------------
StraightSkeleton::StraightSkeleton()
{
}

//--------------------------------------------------------------
StraightSkeleton::~StraightSkeleton()
{
}


//--------------------------------------------------------------
void StraightSkeleton::CreateSkeleton(ofPolyline& polygon)
{
	// compare function for priority queue (compare distance)
	auto cmp = [](Event left, Event right) { return left.distance < right.distance; };
	
	// construct event queue
	priority_queue<Event, vector<Event>, decltype(cmp)> eventQueue;

	SLAV slav = SLAV(polygon);

	eventQueue.

	// TODO: fill event queue emplace()
	for (size_t i = 0; i < slav.activeLavs.size(); i++)
	{
		// add events for every LAV
		eventQueue.emplace(slav.activeLavs[i].getEvents());
		// get events for this lav
		
		//for (size_t j = 0; j < slav.activeLavs[i].vertices.size(); j++)
		//{
		//	// add next vertex event
		//	eventQueue.emplace()
		//	// GetNextEvent(node)
		//	//eventQueue.emplace()
		//}

	}

	// Handle events
	while (!eventQueue.empty())
	{
		// get top event
		int event = eventQueue.top();
		
		// handle edge event


		// handle split event
	}
}