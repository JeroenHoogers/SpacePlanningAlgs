#pragma once

#include "ofMain.h"
#include "IntersectionHelper.h"

class LAV;
class SLAV;

struct LineSegment;
struct Event;

typedef tuple<vector<LineSegment>, vector<Event>> EventOutput;
typedef tuple<vector<LineSegment>, vector<ofPolyline>> SSAlgOutput;

//// Arc structure
//struct SkeletonArc
//{
//
//};

struct LineSegment
{
	ofPoint v1;
	ofPoint v2;

	ofVec2f dir1;
	ofVec2f dir2;

	ofVec2f bisectorLeft;
	ofVec2f bisectorRight;

	LineSegment()
	{

	}

	LineSegment(ofPoint _v1, ofPoint _v2)
	{
		v1 = _v1;
		v2 = _v2;

		dir1 = (v2 - v1).normalized();
		dir2 = (v1 - v2).normalized();
	}

	LineSegment(ofPoint _v1, ofPoint _v2, ofVec2f _bisectorLeft, ofVec2f _bisectorRight)
	{
		v1 = _v1;
		v2 = _v2;

		dir1 = (v2 - v1).normalized();
		dir2 = (v1 - v2).normalized();

		bisectorLeft = _bisectorLeft;
		bisectorRight = _bisectorRight;
	}

	bool operator==(const LineSegment& rhs) const
	{
		return (this->v1 == rhs.v1 && this->v2 == rhs.v2);
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

	LineSegment oppositeEdge;

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
	Event(float _distance, ofPoint _intersection, struct Node* _v, LineSegment _oppositeEdge)
	{
		distance = _distance;
		intersection = _intersection;
		v1 = _v;
		v2 = _v;

		oppositeEdge = _oppositeEdge;

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

	struct LineSegment edgeLeft;
	struct LineSegment edgeRight;

	struct Node* prev;
	struct Node* next;

	LAV* pLav;

	//Node(ofPoint vert, LAV* lav)
	//{
	//	p = vert;
	//	pLav = lav;
	//	active = true;
	//}

	Node(ofPoint vert, LAV* lav, LineSegment _edgeLeft, LineSegment _edgeRight)
	{
		p = vert;
		pLav = lav;
		active = true;

		edgeLeft = _edgeLeft;
		edgeRight = _edgeRight;
	}


	// reflex
	bool isReflex()
	{
		ofVec2f e1 = (prev->p - p).normalized();
		ofVec2f e2 = (next->p - p).normalized();

		float d = IntersectionHelper::det(e1, e2);

		return d < 0;
	}

	// compute bisector
	void computeBisector()
	{
		// add edge vectors to obtain bisector
		bisector = edgeLeft.dir2 + edgeRight.dir1;
		bisector.normalize();

		if (isReflex())
			bisector *= -1;
	}

	// get the next event for this vertex
	bool getNextEvent(Event* nextEvent);
};

//--------------------------------------------------------------
class LAV
{
public:
	struct Node* head = NULL;
	int length = 0;
	SLAV* pSlav;

	// construct from polygon
	LAV(const ofPolyline& polygon, SLAV* slav)
	{
		pSlav = slav;

		// create double connected circular list
		for (size_t i = 0; i < polygon.size(); i++)
		{
			int prev = polygon.getWrappedIndex(i - 1);
			int next = polygon.getWrappedIndex(i + 1);

			// create node
			addNode(&head, polygon[i], LineSegment(polygon[prev], polygon[i]), LineSegment(polygon[i], polygon[next]));
		}

		computeBisectors();
	};

	// Construct LAV from chain
	LAV(Node* _head, SLAV* slav)
	{
		struct Node* v = _head;
		head = _head;	
		length = 1;

		v = v->next;

		// check how many vertices are in the chain
		while (v != head)
		{
			length++;
			v = v->next;

			// guard against infinite loop (if length becomes too big)
			if (length > 100)
				break;
		}
	}

	~LAV()
	{
		//if (head != NULL)
		//{
		//	struct Node* s = head;
		//	

		//	for (size_t i = 0; i < length; i++)
		//	{
		//		struct Node* n = s->next;

		//		delete s;

		//		// go to the next value
		//		s = n;
		//	}
		//}
	}

	bool operator==(const LAV& rhs) const 
	{ 
		return this->head == rhs.head;
	}

	// unify 2 vertices by replacing them in the chain with a new vertex
	Node* unify(struct Node* v1, struct Node* v2, ofVec3f vert)
	{
		// create replacement node
		struct Node* node = new Node(vert, this, v1->edgeLeft, v2->edgeRight);

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
		//node->prev->computeBisector();
		//node->next->computeBisector();

		ofSetLineWidth(1.0f);
		ofSetColor(100, 100, 100);
		ofDrawLine(node->p, node->p + node->bisector * 500);

		return node;
	}

	bool empty() const
	{
		return length == 0;
	}

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
				if (s->getNextEvent(&e))
					events.push_back(e);

				// go to the next value
				s = s->next;
			}
		}

		return events;
	}

private:
	void addNode(struct Node** start, ofPoint vert, LineSegment edgeLeft, LineSegment edgeRight)
	{
		length++;

		// List is empty, add first element
		if (*start == NULL)
		{
			struct Node* node = new Node(vert, this, edgeLeft, edgeRight);
			node->prev = node->next = node;
			*start = node;

			return;
		}

		// find last node
		Node *last = (*start)->prev;

		// create new node
		struct Node* node = new Node(vert, this, edgeLeft, edgeRight);
		node->next = *start;

		// the new node becomes the previous of start
		(*start)->prev = node;

		// make last node previous of the new node
		node->prev = last;

		// make new node next of last node
		last->next = node;

		//// update bisectors
		//(*start)->computeEdges();
		//node->computeEdges();
		//last->computeEdges();

		//(*start)->computeBisector();
		//node->computeBisector();
		//last->computeBisector();
	}

	void computeBisectors()
	{
		if (head != NULL)
		{
			struct Node* s = head;
			for (size_t i = 0; i < length; i++)
			{
				//s->computeEdges();
				s->computeBisector();

				ofSetLineWidth(1.0f);
				ofSetColor(100, 100, 100);
				ofDrawLine(s->p, s->p + s->bisector * 500);

				// go to the next value
				s = s->next;
			}
		}
	}
};

//--------------------------------------------------------------
class SLAV
{
public:
	vector<LAV*> activeLavs;
	vector<LineSegment> originalEdges;

	SLAV(const ofPolyline& polygon)
	{
		activeLavs = vector<LAV*>();
		activeLavs.push_back(new LAV(polygon, this));

		// store the original edges
		for (size_t i = 0; i < polygon.size(); i++)
		{
			int prev2 = polygon.getWrappedIndex(i - 2);
			int prev = polygon.getWrappedIndex(i - 1);
			int next = polygon.getWrappedIndex(i + 1);

			ofVec2f eEdge = (polygon[i] - polygon[prev]).normalized();
			ofVec2f ePrev = (polygon[prev2] - polygon[prev]).normalized();
			ofVec2f eNext = (polygon[next] - polygon[i]).normalized();

			// compute bisectors
			ofVec2f bisectorLeft = (ePrev * -1 + eEdge).normalized();
			ofVec2f bisectorRight = (eEdge + eNext).normalized();

			// flip bisector for reflex vertices
			bool leftReflex = (IntersectionHelper::det(eEdge, ePrev) < 0);
			bool rightReflex = (IntersectionHelper::det(eEdge * -1, eNext) < 0);
			
			bisectorLeft *= -1;
			bisectorRight *= -1;

			// construct line segments
			LineSegment lineSegment = LineSegment(polygon[prev], polygon[i], bisectorLeft, bisectorRight);
			originalEdges.push_back(lineSegment);
		}
	};

	bool empty()
	{
		return activeLavs.size() == 0;
	};

	// handle edge event
	EventOutput HandleEdgeEvent(Event e);

	// handle split event
	EventOutput HandleSplitEvent(Event e);

	
	vector<ofPolyline> ConstructFaces(vector<LineSegment> segments);
};

//--------------------------------------------------------------
static class StraightSkeleton
{

public:

	// Compute the straight skeleton of the polygon
	static SSAlgOutput CreateSkeleton(ofPolyline& polygon, int steps = 200);

};

