#pragma once

#include "ofMain.h"
#include "IntersectionHelper.h"

class LAV;

struct SkeletonArc;
struct Event;


typedef tuple<vector<SkeletonArc>, vector<Event>> EventOutput;

// Arc structure
struct SkeletonArc
{
	ofPoint v1;
	ofPoint v2;

	SkeletonArc(ofPoint _v1, ofPoint _v2)
	{
		v1 = _v1;
		v2 = _v2;
	}
};

enum struct EEventType
{
	EdgeEvent,
	SplitEvent
};

struct Event
{
	// TODO: add LAV?
	//LAV* pLav;

	float distance; // time
	ofPoint intersection;

	EEventType type;

	struct Node* v1;
	struct Node* v2;

	//virtual ~Event() {};

	Event()
	{
		distance = INFINITY;
		intersection = ofPoint();
	}

	// edge event constructor
	Event(float _distance, ofPoint _intersection, struct Node* _v1, struct Node* _v2)
	{
		distance = _distance;
		intersection = _intersection;
		v1 = _v1;
		v2 = _v2;

		type = EEventType::EdgeEvent;
	}

	// split event constructor
	Event(float _distance, ofPoint _intersection, struct Node* _v)
	{
		distance = _distance;
		intersection = _intersection;
		v1 = _v;
		v2 = _v;

		type = EEventType::SplitEvent;
	}

	// TODO: sort function
	bool operator()(const Event& lhs, const Event& rhs) const 
	{ 
		return lhs.distance < rhs.distance;
	};

	bool operator<(const Event& rhs) const
	{
		return distance < rhs.distance;
	};

	bool operator>(const Event& rhs) const
	{
		return distance > rhs.distance;
	};
};

//struct EdgeEvent : public Event
//{
//	struct Node* v1;
//	struct Node* v2;
//
//	EdgeEvent(struct Node* _v1, struct Node* _v2, float _distance, ofPoint _intersection) : Event(_distance, _intersection)
//	{
//		v1 = _v1;
//		v2 = _v2;
//		//distance = _distance;
//		//intersection = _intersection;
//	}
//};
//
//struct SplitEvent : public Event
//{
//	struct Node* v;
//};

//--------------------------------------------------------------
struct Node
{
	ofPoint p;
	ofVec2f bisector;
	bool active;

	struct Node* prev;
	struct Node* next;

	LAV* pLav;


	Node(ofPoint vert, LAV* lav)
	{
		p = vert;
		pLav = lav;
		active = true;
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
		bisector.normalize();

		if (isReflex())
			bisector *= -1;
	}

	bool getNextEvent(Event* nextEvent)
	{
		bool hasEvent = false;
		float mindist = INFINITY;

		bool reflex = isReflex();

		if(reflex)
		{
			// TODO: Check for split events

		}

		// check for intersection with previous bisector
		ofPoint intersection;
		if (IntersectionHelper::intersectRays(p, bisector, prev->p, prev->bisector, &intersection))
		{
			float dist = p.distance(intersection);
			if (mindist > dist)
			{
				// update next event
				mindist = dist;
				hasEvent = true;
				(*nextEvent) = Event(dist, intersection, this, prev); // edge event
			}
		}
		
		// check for intersection with next bisector
		if (IntersectionHelper::intersectRays(p, bisector, next->p, next->bisector, &intersection))
		{
			float dist = p.distance(intersection);
			if (mindist > dist)
			{
				// update next event
				mindist = dist;
				hasEvent = true;
				(*nextEvent) = Event(dist, intersection, this, next); // edge event
			}
		}

		// TODO: handle the case where there are no events
		
		return hasEvent;
	}
};

//--------------------------------------------------------------
class LAV
{
public:
	struct Node* head = NULL;
	int length = 0;

	LAV(const ofPolyline& polygon)
	{
		// create double connected circular list
		for (size_t i = 0; i < polygon.size(); i++)
		{
			addNode(&head, polygon[i]);
		}
	};

	~LAV()
	{
		if (head != NULL)
		{
			struct Node* s = head;
			

			for (size_t i = 0; i < length; i++)
			{
				struct Node* n = s->next;

				delete s;

				// go to the next value
				s = n;
			}
		}
	}

	bool operator==(const LAV& rhs) const 
	{ 
		return this->head == rhs.head;
	}

	// unify 2 vertices by replacing them in the chain with a new vertex
	Node* unify(struct Node* v1, struct Node* v2, ofVec3f vert)
	{
		// create replacement node
		struct Node* node = new Node(vert, this);

		if (head == v1 || head == v2)
			head = node;

		// rearrange links to replace v1, v2 with the new node
		v1->prev->next = node;
		v2->next->prev = node;
		node->prev = v1->prev;
		node->next = v2->next;

		length--;

		// delete nodes
		// causes memory leak
		v1->active = false;
		v2->active = false;
	//	delete v1;
	//	delete v2;
		node->computeBisector();
		node->prev->computeBisector();
		node->next->computeBisector();

		return node;
	}

	void addNode(struct Node** start, ofPoint vert)
	{
		length++;

		// List is empty, add first element
		if (*start == NULL)
		{
			struct Node* node = new Node(vert, this);
			node->prev = node->next = node;
			*start = node;

			return;
		}

		// find last node
		Node *last = (*start)->prev;

		// create new node
		struct Node* node = new Node(vert, this);
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

	bool empty() const
	{
		return length == 0;
	};

	vector<Event> getEvents() const
	{
		vector<Event> events;

		// loop vertices
		if (head != NULL)
		{
			struct Node* s = head;
			for (size_t i = 0; i < length; i++)
			{
				Event e;

				// if the vertex has events, add them to the list
				if(s->getNextEvent(&e))
					events.push_back(e);

				// go to the next value
				s = s->next;
			}
		}

		return events;
	}
};

//--------------------------------------------------------------
class SLAV
{
public:
	vector<LAV*> activeLavs;

	SLAV(const ofPolyline& polygon)
	{
		activeLavs = vector<LAV*>();
		activeLavs.push_back(new LAV(polygon));
		//boundary = polygon;
	};

	bool empty()
	{
		return activeLavs.size() == 0;
	};

	// handle edge event
	EventOutput HandleEdgeEvent(Event e);

	EventOutput HandleSplitEvent(Event e);

};

//--------------------------------------------------------------
static class StraightSkeleton
{

public:

	// Compute the straight skeleton of the polygon
	static vector<SkeletonArc> CreateSkeleton(ofPolyline& polygon);

};

