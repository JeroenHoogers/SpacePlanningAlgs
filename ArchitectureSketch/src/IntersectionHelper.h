#pragma once
#include "ofMain.h"

static class IntersectionHelper
{
public:
	
	bool approx(float a, float b)
	{
		return (a == b) || (abs(a - b) <= 0.001f);
	}

	static float det(const ofVec2f& v1, const ofVec2f& v2)
	{
		return v1.x * v2.y - v1.y * v2.x;
	}

	static bool intersectLines(const ofPoint& p1, const ofVec2f& d1, const ofPoint& p2, const ofVec2f& d2, ofPoint* intersection)
	{
		//ofSetColor(255, 20, 20);
		//ofLine(p1, p1 + d1 * 50);
		//ofLine(p2, p2 + d2 * 50);

		float d = det(d1, d2);
		//float det1 = d2.y * d1.x - d2.x * d1.y;

		// check if lines are parallel
		if (d == 0)
			return false;

		ofVec2f diff = p2 - p1;

		float u = det(diff, d2) / d;
		float v = det(diff, d1) / d;

		//if (u >= 0 && v >= 0)
		//{
		ofPoint i = p1 + d1 * u;
		(*intersection) = i;

			//ofDrawCircle(i, 3.0f);
			//ofSetColor(100, 100, 100);

			return true;
		//}

		//return false;
	}

	static bool intersectRays(const ofPoint& p1, const ofVec2f& d1, const ofPoint& p2, const ofVec2f& d2, ofPoint* intersection)
	{
		//ofSetColor(255, 20, 20);
		//ofLine(p1, p1 + d1 * 50);
		//ofLine(p2, p2 + d2 * 50);

		float d = det(d1, d2);
		//float det1 = d2.y * d1.x - d2.x * d1.y;
		if (d == 0)
			return false;


		ofVec2f diff = p2 - p1;

		float u = det(diff, d2) / d;
		float v = det(diff, d1) / d;


		//ofVec2f diff = p2 - p1;
	/*	float u = (d2.x * dy - d2.y - dx) / det;
		float v = (d1.x * dy - d1.y - dx) / det;*/

		//ofLine(p1, p1 + d1 * u);
		//ofLine(p2, p2 + d2 * v);

		if (u >= 0 && v >= 0)
		{
			ofPoint i = p1 + d1 * u;
			(*intersection) = i;

			ofDrawCircle(i, 3.0f);
			ofSetColor(100, 100, 100);

			return true;
		}

		// if both u and v are positive, the rays intersect
		//if (u >= 0 && v >= 0)
		//{
		//	ofPoint i = ofPoint(p1.x + u * d1.x, p1.y + u * d1.y);

		//	ofDrawCircle(i, 3.0f);
		//	ofSetColor(100, 100, 100);

		//	(*intersection) = i;
		////	(*intersection) = p1 + d1 * u;
		//	//(*intersection) = p2 + d2 * u;
		//	return true;
		//}

		return false;
	}

	static int ccw(const ofPoint& p1, const ofPoint& p2, const ofPoint& q) 
	{
		return (p2.x - p1.x) * (q.y - p1.y) - (q.x - p1.x) * (p2.y - p1.y);
	}

	static bool intersects(const ofPoint& p1, const ofPoint& p2, const ofPoint& q1, const ofPoint& q2) 
	{
		if (ccw(p1, p2, q1) * ccw(p1, p2, q2) > 0) return false;
		if (ccw(q1, q2, p1) * ccw(q1, q2, p2) > 0) return false;
		return true;
	}

	// Checks if a point q lies on the segment (p1,p2) given that p1, p2 and q are collinear
	static bool onSegment(const ofPoint& p1, const ofPoint& p2, const ofPoint& q)
	{
		float eps = 0.05;
		if (q.x <= max(p1.x, p2.x) + eps && q.x >= min(p1.x, p2.x) - eps &&
			q.y <= max(p1.y, p2.y) + eps && q.y >= min(p1.y, p2.y) - eps)
			return true;

		return false;
	};

	// Determine the orientation of q with respect to line segment p1 p2 q
	// 0 = p1, p2 and q are collinear 
	// 1 = Clockwise
	// 2 = Counterclockwise
	static int orientation(const ofPoint& p1, const ofPoint& p2, const ofPoint& q)
	{
		float det =
			(p2.y - p1.y) * (q.x - p2.x) -
			(p2.x - p1.x) * (q.y - p2.y);

		if (abs(det) == 0) return 0; // collinear

		return (det > 0) ? 1 : 2; // CW or CCW
	}


	// Returns the relative angle between vectors p1->p2 and p1->q 
	// Range of the angle is beween [-PI, PI]

	static float relativeAngle(const ofVec2f& v1, const ofVec2f& v2)
	{
		float dot = v1.dot(v2);
		float theta = acos(dot);

		ofVec2f v1perp = ofVec2f(-v1.y, v1.x);

		if (v2.dot(v1perp) > 0)
			theta = PI * 2 - theta;

		//float dot = v1.dot(v2);

		//float theta = atan2f(v1.x * v2.y - v1.y * v2.x, v1.x * v2.x + v1.y * v2.y);
		
		//float theta1 = atan2f(v1.y, v1.x);
		//float theta2 = atan2f(v2.y, v2.x);
		// atan2 ranges from [-PI,PI], we would like to correct this discontinuity
		//theta1 = (theta1 > 0.0 ? theta1 : (PI * 2 + theta1));
		//theta2 = (theta2 > 0.0 ? theta2 : (PI * 2 + theta2));

		//float theta = (theta1 - theta2);
		//theta = (theta > 0.0 ? theta : (PI*2 + theta));
		
		//theta += PI;
		if (theta > PI * 2 - 0.2) 
			theta = 0;
		
		return theta;
	}

	// Determines if line segments (p1,p2) and (q1,q2) intersect
	static bool doIntersect(const ofPoint& p1, const ofPoint& p2, const ofPoint& q1, const ofPoint& q2)
	{
		// find orientations
		int o1 = orientation(p1, p2, q1);
		int o2 = orientation(p1, p2, q2);
		int o3 = orientation(q1, q2, p1);
		int o4 = orientation(q1, q2, p2);

		// general case 
		if (o1 != o2 && o3 != o4)
			return true;

		// special cases when 3 points are colinear we have to check whether an endpoint touches the line segment
		//if (o1 == 0 && onSegment(p1, p2, q1)) return true;
		//if (o2 == 0 && onSegment(p1, p2, q2)) return true;
		//if (o3 == 0 && onSegment(q1, q2, p1)) return true;
		//if (o4 == 0 && onSegment(q1, q2, p2)) return true;

		return false;
	};

	static float getDistanceToLine(const ofPoint& v1, const ofPoint& v2, const ofPoint& p)
	{
		ofVec2f diff = v2 - v1;
		float d = det(p, diff);

		float distance = abs(d + det(v2, v1)) / diff.length();
		
		// DEBUG: draw distance
		//ofDrawBitmapString(ofToString(distance), p);

		//u.normalize();
		return distance;
	}

	static float getDistanceToEdge(const ofPoint& v1, const ofPoint& v2, const ofPoint& p)
	{
		// calculate difference vectors
		ofVec2f u = v2 - v1;
		ofVec2f v = p - v1;

		// calculate interpolation variable t corresponding 
		float t = u.dot(v) / v.lengthSquared();

		// clamp t between 0 and 1 since the projected point must lie on the edge
		t = ofClamp(t, 0, 1);

		// calculate the projection of p on the edge
		ofPoint q = v1 + u * t;

		ofSetColor(255, 30, 30);
		ofLine(p, q);

		ofLine(v1, p);
		ofLine(v2, p);

		float d2 = getDistanceToLine(v1, v2, p);
		float dist = p.distance(q);
		ofDrawBitmapString(ofToString(dist), p);

		// return the squared distance from p to q
		return dist;
	}

	static ofVec2f getProjectedPointOnLine(ofPoint v1, ofPoint v2, ofPoint p)
	{
		// calculate difference vectors
		ofVec2f e1 = v2 - v1;
		ofVec2f e2 = p - v1;

		float dot = e1.dot(e2);

		// projection vector along e1
		float t = dot / e1.lengthSquared();
		t = ofClamp(t, 0, 1);
		//t = Mathf.Clamp01(t);

		//ofVec2f e2Proj = (dot * e1) / e1.lengthSquared();
		// calculate the projection of p on the edge
		ofPoint q = v1 + e1 * t;

		// p projected onto the line intersecting v1 and v2
		//if (onSegment(v1, v2, q))
			return q;
		//else
		//	return ofPoint(0, 0);
	};
};