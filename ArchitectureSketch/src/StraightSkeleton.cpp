#include "StraightSkeleton.h"

//--------------------------------------------------------------
bool Node::getNextEvent(Event* nextEvent)
{
	bool hasEvent = false;
	float mindist = INFINITY;

	bool reflex = isReflex();

	// Detect split events for reflex vertices
	if (reflex && false)
	{
		SLAV* slav = pLav->pSlav;

		// Check for split events
		for (size_t i = 0; i < slav->originalEdges.size(); i++)
		{
			LineSegment edge = slav->originalEdges[i];

			if (edge == edgeLeft || edge == edgeRight)
				continue;

			// compute dot products
			float lDot = fabsf(edgeLeft.dir1.dot(edge.dir1));
			float rDot = fabsf(edgeRight.dir1.dot(edge.dir1));

			LineSegment selfEdge = (lDot < rDot) ? edgeLeft : edgeRight;
			LineSegment otherEdge = (lDot > rDot) ? edgeLeft : edgeRight;

			// TODO: check intersection
			ofPoint intersection;
			if (IntersectionHelper::intersectLines(selfEdge.v1, selfEdge.dir1, edge.v1, edge.dir1, &intersection) &&
				!IntersectionHelper::approx(intersection, p))
			{
				ofVec2f linVec = (p - intersection).normalized();
				ofVec2f edgeVec = edge.dir1;

				if (linVec.dot(edgeVec) < 0)
					edgeVec = -edgeVec;

				ofVec2f bisecVec = edgeVec + linVec;

				// if the bisector has a length of 0 stop
				if (bisecVec.length() == 0) 
					continue;

				ofPoint bIntersection;
				if (IntersectionHelper::intersectRays(intersection, bisecVec, p, bisector, &bIntersection))
				{
					// the intersection should lie in the area marked by the edge and bisectors of two vertices
					float detLeft = IntersectionHelper::det(edge.bisectorLeft, (bIntersection - edge.v1).normalized()); 
					float detRight = IntersectionHelper::det(edge.bisectorRight, (bIntersection - edge.v2).normalized());
					float detEdge = IntersectionHelper::det(edge.dir1, (bIntersection - edge.v1).normalized());

					// check if the intersection lies inside the area marked by the edges 
					if (detLeft > 0 && detRight < 0 && detEdge < 0)
					{
						float dist = p.distance(bIntersection);
						float time = IntersectionHelper::getDistanceToLine(edge.v1, edge.v2, bIntersection);

						if (mindist > dist)
						{
							mindist = dist;
							hasEvent = true;
							(*nextEvent) = Event(time, bIntersection, this, edge); // split event
						}
					}
				}
			}
		}
	}

	// check for intersection with previous bisector
	ofPoint intersection;
	if (IntersectionHelper::intersectRays(p, bisector, prev->p, prev->bisector, &intersection))
	{
		float dist = p.distance(intersection);
		float time = IntersectionHelper::getDistanceToLine(edgeLeft.v1, edgeLeft.v2, intersection);

		if (mindist > dist)
		{
			// update next event
			mindist = dist;
			hasEvent = true;
			(*nextEvent) = Event(time, intersection, prev, this); // edge event
		}
	}

	// check for intersection with next bisector
	if (IntersectionHelper::intersectRays(p, bisector, next->p, next->bisector, &intersection))
	{
		float dist = p.distance(intersection);
		float time = IntersectionHelper::getDistanceToLine(edgeRight.v1, edgeRight.v2, intersection);

		if (mindist > dist)
		{
			// update next event
			mindist = dist;
			hasEvent = true;
			(*nextEvent) = Event(time, intersection, this, next); // edge event
		}
	}

	if ((*nextEvent).type == EEventType::SplitEvent)
		float i = 0;

	// TODO: handle the case where there are no events
	return hasEvent;
}

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
		i.z = e.distance;

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
		if (find(activeLavs.begin(), activeLavs.end(), pLav) != activeLavs.end())
		{
			activeLavs.erase(
				find(activeLavs.begin(), activeLavs.end(), pLav));
		}

		// delete lav
		delete pLav;
	}
	else // edge event
	{
		// add arcs (v1, i) and (v2, i)	
		ofPoint v1 = e.v1->p;
		ofPoint v2 = e.v2->p;

		// compute intersection point (store distance in z-coord)
		ofPoint i = e.intersection;
		i.z = e.distance;

		struct Node* newNode = pLav->unify(e.v1, e.v2, i);

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

	LAV* pLav = e.v1->pLav;

	vector<struct Node*> vertices;

	struct Node* vLeft = nullptr;
	struct Node* vRight = nullptr;

	ofVec2f norm = e.oppositeEdge.dir1;

	// loop all lavs
	for (size_t i = 0; i < activeLavs.size(); i++)
	{
		LAV* curLav = activeLavs[i];

		if(curLav->head != NULL)
		{
			struct Node* v = curLav->head;

			// loop all vertices in the lav
			for (size_t j = 0; j < curLav->length; j++)
			{
				if (norm == v->edgeLeft.dir1 && e.oppositeEdge.v1 == v->edgeLeft.v1)
				{
					vLeft = v;
					vRight = vLeft->prev;
				}
				else if (norm == v->edgeRight.dir1 && e.oppositeEdge.v1 == v->edgeRight.v1)
				{
					vRight = v;
					vLeft = vRight->next;
				}

				if (vLeft != nullptr)
				{
					float detLeft = IntersectionHelper::det(vRight->bisector, (e.intersection - vRight->p).normalized());
					float detRight = IntersectionHelper::det(vLeft->bisector, (e.intersection - vLeft->p).normalized());

					// Check whether we have found the
					if (detLeft >= 0 && detRight <= 0)
						break;
					else
					{
						vLeft = nullptr;
						vRight = nullptr;
					}
				}

				// proceed to next vertex in the lav
				v = v->next;
			}
		}
	}

	// check whether suitable vertices vLeft and vRight have been found
	if (vLeft == nullptr)
		return EventOutput(arcs, events);

	// add arc to the list
	arcs.push_back(LineSegment(e.v1->p, e.intersection));

	// construct replacement vertices v1 and v2
	// TODO: make sure LAV reference is right
	struct Node* v1 = new Node(e.intersection, pLav, e.v1->edgeLeft, e.oppositeEdge);
	struct Node* v2 = new Node(e.intersection, pLav, e.oppositeEdge, e.v1->edgeRight);
	
	// insert new nodes into the correct place in the LAV's
	v1->prev = e.v1->prev;
	v1->next = vLeft;
	e.v1->prev->next = v1;
	vLeft->prev = v1;

	v2->prev = vRight;
	v2->next = e.v1->next;
	e.v1->next->prev = v2;
	vRight->next = v2;

	// create new lavs
	vector<LAV*> newLavs;
	
	// delete current lav
	//if (find(activeLavs.begin(), activeLavs.end(), pLav) != activeLavs.end())
	//{
		activeLavs.erase(
			find(activeLavs.begin(), activeLavs.end(), pLav));
	//}

	if (pLav != vLeft->pLav)
	{
		// split event merges 2 lavs
		//if (find(activeLavs.begin(), activeLavs.end(), vLeft->pLav) != activeLavs.end())
		//{
			activeLavs.erase(
				find(activeLavs.begin(), activeLavs.end(), vLeft->pLav));
		//}

		newLavs.push_back(new LAV(v1, this));
	}
	else
	{
		// split into 2 lavs
		newLavs.push_back(new LAV(v1, this));
		newLavs.push_back(new LAV(v2, this));
	}

	// Fix Lavs
	for (size_t i = 0; i < newLavs.size(); i++)
	{
		// lav has 3 or more vertices, proceed with the algorithm
		if (newLavs[i]->length > 2)
		{
			activeLavs.push_back(newLavs[i]);
			vertices.push_back(newLavs[i]->head);
		}
		else // lav has collapsed into a line
		{
			// add arc
			arcs.push_back(LineSegment(newLavs[i]->head->next->p, e.intersection));
			struct Node* v = newLavs[i]->head;

			// loop all vertices in the lav and set them to inactive
			for (size_t j = 0; j < newLavs[i]->length; j++)
			{
				v->active = false;
				v = v->next;
			}
		}
	}

	// loop vertices and compute their events
	for (size_t i = 0; i < vertices.size(); i++)
	{
		// get next event and add it to the event list
		Event nextEvent;
		if (vertices[i]->getNextEvent(&nextEvent))
			events.push_back(nextEvent);
	}

	e.v1->active = false;

	return EventOutput(arcs, events);
}

//--------------------------------------------------------------
vector<ofPolyline> SLAV::ConstructFaces(vector<LineSegment> segments)
{
	vector<ofPolyline> faces;

	// loop original edges to construct faces
	for (size_t i = 0; i < originalEdges.size(); i++)
	{
		ofPolyline face;
		face.addVertex(originalEdges[i].v1);
		face.addVertex(originalEdges[i].v2);

		bool done = false;

		while (!done)
		{
			// loop line segments to find the next edge
			ofPoint pCur = face[face.size() - 1];
			ofPoint pPrev = face[face.size() - 2];

			// store candidate to find the leftmost point compared to the last edge
			ofPoint candidate;
			bool hasCandidate = false;
			//float minAngle = 0;

			for (size_t j = 0; j < segments.size(); j++)
			{
				ofPoint pNext = ofPoint();
				bool connected = false;
				if (IntersectionHelper::approx(segments[j].v1, pCur) && !IntersectionHelper::approx(segments[j].v2, pPrev))
				{
					pNext = segments[j].v2;
					connected = true;
				}
				else if (IntersectionHelper::approx(segments[j].v2,pCur) && !IntersectionHelper::approx(segments[j].v1, pPrev))
				{
					pNext = segments[j].v1;
					connected = true;
				}

				if (connected)
				{

					//float angle = IntersectionHelper::relativeAngle((pCur - pPrev).normalized(), (pNext - pCur).normalized());
					
					// if the angle is less then our current candidate, store this candidate
					if (!hasCandidate)
					{
						hasCandidate = true;
						candidate = pNext;
						//minAngle = angle;
					}
					else  // check if this point lies left of the line segment 
					{
						if (IntersectionHelper::orientation(pPrev, pCur, pNext) != IntersectionHelper::orientation(pPrev, pCur, candidate))
						{
							if (IntersectionHelper::orientation(pPrev, pCur, pNext) == 1)
							{
								hasCandidate = true;
								candidate = pNext;
							}
						}
						else if (IntersectionHelper::orientation(pCur, candidate, pNext) == 1)
						{
							hasCandidate = true;
							candidate = pNext;
						}
					}
				}
			}

			// add next vertex to polyline
			if (hasCandidate && face.size() < 25)
			{
				if (IntersectionHelper::approx(candidate, originalEdges[i].v2))
				{
					done = true;
					face.close();
				}
				else
					face.addVertex(candidate);
			}
			else
			{
				// this is most likely the last vertex, check if it connects to the original edge
				done = true;
		//		face.close();
			}
		}

		faces.push_back(face);
	}

	return faces;
}

//--------------------------------------------------------------
SSAlgOutput StraightSkeleton::CreateSkeleton(ofPolyline& polygon, int steps)
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

		if (steps == 0)
			ofDrawBitmapString("type: " + (e.type == EEventType::EdgeEvent) ? "edge" : "split", ofPoint(10,10));
	}

	// construct faces
	vector<ofPolyline> faces = slav.ConstructFaces(skeleton);

	return SSAlgOutput(skeleton, faces);
}