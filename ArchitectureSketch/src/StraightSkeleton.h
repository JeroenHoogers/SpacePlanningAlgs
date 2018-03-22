#pragma once

#include "ofMain.h"
#include "IntersectionHelper.h"

// Arc structure
struct LineArc
{
	// vertex index
	int index1;
	int index2;
};

struct Event
{
	float distance; // time
	ofPoint intersection;

	// TODO: sort function
};

struct EdgeEvent : Event
{
	struct Node* v1;
	struct Node* v2;

	EdgeEvent(struct Node* _v1, struct Node* _v2, float _distance, ofPoint _intersection)
	{
		v1 = _v1;
		v2 = _v2;
		distance = _distance;
		intersection = _intersection;
	}
};

struct SplitEvent : Event
{
	struct Node* v;
};


// Line graph datastructure 
struct LineGraph
{
	ofPolyline polygon;

	vector<LineArc> nodes;
};

//--------------------------------------------------------------
struct Node
{
	ofPoint p;
	ofVec2f bisector;
	int lavId;

	struct Node* prev;
	struct Node* next;


	Node(ofPoint vert, int lav)
	{
		p = vert;
		lavId = lav;
	}
	// TODO: compute bisector

	bool isReflex()
	{
		ofVec2f e1 = (prev->p - p).normalized();
		ofVec2f e2 = (next->p - p).normalized();

		float cross = e1.x*e2.y - e2.x*e1.y;

		return cross < 0;
	}

	void computeBisector()
	{
		ofVec2f e1 = (prev->p - p).normalized();
		ofVec2f e2 = (next->p - p).normalized();

		// add edge vectors to obtain bisector
		bisector = e1 + e2;

		if (isReflex())
			bisector *= -1;
	}

	Event getNextEvent()
	{
		Event nextEvent;
		float mindist = INFINITY;

		if(isReflex())
		{
			// TODO: Check for split events

		}

		// check for intersection with previous bisector
		ofPoint intersection;
		if (IntersectionHelper::intersectRays(p, bisector, prev->p, prev->bisector, &intersection))
		{
			float dist = p.distance(intersection);
			if (mindist < dist)
			{
				// update next event
				mindist = dist;
				nextEvent = EdgeEvent(this, prev, dist, intersection);
			}
		}
		
		// check for intersection with next bisector
		if (IntersectionHelper::intersectRays(p, bisector, next->p, next->bisector, &intersection))
		{
			float dist = p.distance(intersection);
			if (mindist < dist)
			{
				// update next event
				mindist = dist;
				nextEvent = EdgeEvent(this, next, dist, intersection);
			}
		}

		// TODO: handle the case where there are no events
		
		return nextEvent;
	}
};

//--------------------------------------------------------------
class LAV
{
public:
	struct Node* head = NULL;
	int length = 0;
	int lavId;

	LAV(const ofPolyline& polygon, int id)
	{
		lavId = id;
		// create double connected circular list
		for (size_t i = 0; i < polygon.size(); i++)
		{
			addNode(&head, polygon[0]);
		}
	};

	void addNode(struct Node** start, ofPoint vert)
	{
		length++;

		// List is empty, add first element
		if (*start == NULL)
		{
			struct Node* node = new Node(vert);
			node->prev = node->next = node;
			*start = node;

			return;
		}

		// find last node
		Node *last = (*start)->prev;

		// create new node
		struct Node* node = new Node(vert);
		node->next = *start;

		// the new node becomes the previous of start
		(*start)->prev = node;

		// make last node previous of the new node
		node->prev = last;

		// make new node next of last node
		last->next = node;

		// update bisectors
		(*start)->computeBisector();
		node->computeBisector();
		last->computeBisector();
	};

	Event getNextEventForIndex(int index)
	{
		return Event();
	}

	bool empty()
	{
		return length == 0;
	};

	vector<Event> getEvents()
	{
		vector<Event> events;
		return events;
	}
};

//--------------------------------------------------------------
class SLAV
{
public:
	vector<LAV> activeLavs;

	SLAV(const ofPolyline& polygon)
	{
		activeLavs = vector<LAV>();
		activeLavs.push_back(LAV(polygon, 0));
		//boundary = polygon;
	};

	bool empty()
	{
		return activeLavs.size() == 0;
	};

	// handle edge event
	void HandleEdgeEvent(EdgeEvent e)
	{
		// collapse
		if (e.v1->prev == e.v2->next)
		{
			// remove at index
			activeLavs.erase(activeLavs.begin() + e.v1->lavId);
			/*activeLavs.erase(
				find(activeLavs.begin(), activeLavs.end(), );*/
		}

	}

};

class StraightSkeleton
{

private: 
	priority_queue<Event> eventQueue;

public:
	StraightSkeleton();
	~StraightSkeleton();

	// Compute the straight skeleton of the polygon
	void CreateSkeleton(ofPolyline& polygon);

};

