#include "StraightSkeleton.h"

//--------------------------------------------------------------
EventOutput SLAV::HandleEdgeEvent(Event e)
{
	vector<LineSegment> arcs;
	vector<Event> events;
	 
	LAV* pLav = e.v1->pLav;

	// collapse (peak event)
	if (e.v1->prev == e.v2->next)
	{
		// TODO: add 3 arcs of the peak
		ofPoint v1 = e.v1->p;
		ofPoint v2 = e.v2->p;
		ofPoint v3 = e.v1->prev->p;

		// compute intersection point (store distance in z-coord)
		ofPoint i = e.intersection;
	//	i.z = e.distance;

		// add arcs to the skeleton
		arcs.push_back(LineSegment(v1, i));
		arcs.push_back(LineSegment(v2, i));
		arcs.push_back(LineSegment(v3, i));

		// add arcs (v1, i), (v2, i) and (v3, i)
		// set inactive
		e.v1->active = false;
		e.v2->active = false;
		e.v1->prev->active = false;

		// remove this lav from the list since we have reached the apex
		activeLavs.erase(
			find(activeLavs.begin(), activeLavs.end(), pLav));

		// delete lav
		delete pLav;
	}
	else // edge event
	{
		// add arcs (v1, i) and (v2, i)	
		ofPoint v1 = e.v1->p;
		ofPoint v2 = e.v2->p;

		struct Node* newNode = pLav->unify(e.v1, e.v2, e.intersection);

		// compute intersection point (store distance in z-coord)
		ofPoint i = e.intersection;
	//	i.z = e.distance;

		arcs.push_back(LineSegment(v1, i));
		arcs.push_back(LineSegment(v2, i));

		// get next event and add it to the event list
		Event nextEvent;
		if (newNode->getNextEvent(&nextEvent))
			events.push_back(nextEvent);
	}

	return EventOutput(arcs, events);
}

//--------------------------------------------------------------
EventOutput SLAV::HandleSplitEvent(Event e)
{
	vector<LineSegment> arcs;
	vector<Event> events;

	return EventOutput(arcs, events);
}

//--------------------------------------------------------------
vector<LineSegment> StraightSkeleton::CreateSkeleton(ofPolyline& polygon, int steps)
{
	// construct event queue
	priority_queue<Event, vector<Event>, std::greater<Event>> eventQueue;
	SLAV slav = SLAV(polygon);

	vector<LineSegment> skeleton;

	// TODO: fill event queue emplace()
	for (size_t i = 0; i < slav.activeLavs.size(); i++)
	{
		// add events for this LAV
		vector<Event> lavEvents = slav.activeLavs[i]->getEvents();
		for (size_t j = 0; j < lavEvents.size(); j++)
		{
			eventQueue.emplace(lavEvents[j]);
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
	while (!eventQueue.empty() && !slav.empty() && steps > 0)
	{
		// get top event
		Event e = eventQueue.top();

		// remove top element
		eventQueue.pop();

		EventOutput res;

		// add arcs
		if (e.type == EEventType::EdgeEvent)
		{
			// handle edge event
			if (!e.v1->active || !e.v2->active)
				continue;

			res = slav.HandleEdgeEvent(e);
		}
		else if(e.type == EEventType::SplitEvent)
		{
			if (!e.v1->active)
				continue;

			// handle split event
			res = slav.HandleSplitEvent(e);
		}
		
		vector<LineSegment> arcs;
		vector<Event> events;

		// unpack tuple
		std::tie(arcs, events) = res;

		// append arcs to skeleton
		skeleton.insert(skeleton.end(), arcs.begin(), arcs.end());

		// add new events to the event queue
		for (size_t i = 0; i < events.size(); i++)
		{
			eventQueue.emplace(events[i]);
		}

		steps--;
	}

	return skeleton;
}