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
void SLAV::HandleEdgeEvent(Event e)
{
	vector<ofVec3f> arcs;
	vector<Event> events;

	LAV* lav = e.v1->pLav;

	// collapse (peak event)
	if (e.v1->prev == e.v2->next)
	{
		// TODO: add 3 arcs of the peak
		ofVec3f v1 = e.v1->p;
		ofVec3f v2 = e.v2->p;
		ofVec3f v3 = e.v1->prev->p;

		arcs.push_back(v1);
		arcs.push_back(v2);
		arcs.push_back(v3);

		// add arcs (v1, i), (v2, i) and (v3, i)

		// remove this lav from the list since we have reached the apex
		activeLavs.erase(
			find(activeLavs.begin(), activeLavs.end(), *lav));

	}
	else // edge event
	{
		// add arcs (v1, i) and (v2, i)	
		ofVec3f v1 = e.v1->p;
		ofVec3f v2 = e.v2->p;

		struct Node* newNode = lav->unify(e.v1, e.v2, e.intersection);

		arcs.push_back(v1);
		arcs.push_back(v2);

		// get next event and add it to the event list
		Event nextEvent;
		if (newNode->getNextEvent(&nextEvent))
			events.push_back(nextEvent);
	}

}

//--------------------------------------------------------------
void SLAV::HandleSplitEvent(Event e)
{

}


//--------------------------------------------------------------
void StraightSkeleton::CreateSkeleton(ofPolyline& polygon)
{
	// compare function for priority queue (compare distance)
	//auto cmp = [](Event left, Event right) { return left.distance < right.distance; };

	// construct event queue
	//priority_queue<Event, vector<Event>, function<bool(Event, Event)>> eventQueue([](Event left, Event right) { return left.distance < right.distance; });

	priority_queue<Event, vector<Event>, std::greater<Event>> eventQueue;

	SLAV slav = SLAV(polygon);

	//eventQueue.

	// TODO: fill event queue emplace()
	for (size_t i = 0; i < slav.activeLavs.size(); i++)
	{
		// add events for this LAV
		vector<Event> lavEvents = slav.activeLavs[i].getEvents();
		for (size_t i = 0; i < lavEvents.size(); i++)
		{
			eventQueue.emplace(lavEvents[i]);
		}
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
		Event e = eventQueue.top();

		// remove top element
		eventQueue.pop();

		// add arcs
		if (e.type == EventType::EdgeEvent)
		{
			slav.HandleEdgeEvent(e);
		}
		else if(e.type == EventType::SplitEvent)
		{
			slav.HandleSplitEvent(e);
		}
		//slav.HandleEdgeEvent(dynamic_cast<EdgeEvent>(e));
		
		// handle edge event


		// handle split event
	}
}