#include "DebugState.h"

//--------------------------------------------------------------
DebugState::DebugState()
{
}

//--------------------------------------------------------------
DebugState::~DebugState()
{
}

//--------------------------------------------------------------
void DebugState::stateEnter()
{
	polygon = getSharedData().building.floorShapes[1];
	for (int i = 0; i < polygon.size(); i++)
	{
		polygon[i] += ofPoint(15,15);
		polygon[i] *= 20;
	}
}

//--------------------------------------------------------------
void DebugState::stateExit()
{

}

//--------------------------------------------------------------
void DebugState::setup()
{
	polygon = ofPolyline();
	polygon.addVertex(ofPoint(20, 20));
	polygon.addVertex(ofPoint(350, 20));
	polygon.addVertex(ofPoint(350, 400));
	polygon.addVertex(ofPoint(750, 400));
	polygon.addVertex(ofPoint(750, 700));
	polygon.addVertex(ofPoint(200, 700));
	polygon.addVertex(ofPoint(200, 500));
	polygon.addVertex(ofPoint(20, 500));
	polygon.close();

	//ofRectangle bb = polygon.getBoundingBox();

	//for (int i = 0; i < polygon.size(); i++)
	//{
	//	int prev = polygon.getWrappedIndex(i - 1);
	//	int next = polygon.getWrappedIndex(i + 1);

	//	ofVec2f v1 = polygon[i] - polygon[prev];
	//	ofVec2f v2 = polygon[next] - polygon[i];

	//	float angle = v1.angle(v2);
	//	if (angle < 0)
	//	{
	//		rays.push_back(LineSegment(polygon[prev], polygon[i] + v1 * 5.0f));
	//		rays.push_back(LineSegment(polygon[next], polygon[i] - v2 * 5.0f));

	//		// create x-split
	//		defaultSplits.push_back(
	//			Split((polygon[i].x - bb.getMinX()) / bb.getWidth(), 0)
	//		);

	//		// create y-split
	//		defaultSplits.push_back(
	//			Split((polygon[i].y - bb.getMinY()) / bb.getHeight(), 1)
	//		);
	//	}

	//	//	rays.push_back(LineSegment(ofPoint(50, 50), ofPoint(500, 500)));
	//	//	rays.push_back(LineSegment(ofPoint(200, 50), ofPoint(100, 400)));
	//}

	//constructGrid();
}

void DebugState::constructGrid()
{
	ofRectangle bb = polygon.getBoundingBox();

	// get relevant floorshape 
	float minx = bb.getMinX();
	float miny = bb.getMinY();
	float w = bb.getWidth();
	float h = bb.getHeight();

	// divide floorshape into grid
	int rows = 1;
	int cols = 1;

	vector<float> xs;
	vector<float> ys;

	// add grid boundary positions
	xs.push_back(0);
	xs.push_back(1);

	ys.push_back(0);
	ys.push_back(1);

	vector<GridCell> cells;

	// determine grid dimensions
	for (int i = 0; i < defaultSplits.size(); i++)
	{
		if (defaultSplits[i].axis == 0)
		{
			cols++;
			xs.push_back(defaultSplits[i].position);
		}
		else if (defaultSplits[i].axis == 1)
		{
			rows++;
			ys.push_back(defaultSplits[i].position);
		}
	}

	// sort x and y positions
	sort(xs.begin(), xs.end());
	sort(ys.begin(), ys.end());

	for (int i = 0; i < ys.size() - 1; i++)
	{
		float cy = miny + ys[i] * h;
		float ch = (ys[i + 1] - ys[i]) * h;

		for (int j = 0; j < xs.size() - 1; j++)
		{
			float cx = minx + xs[j] * w;
			float cw = (xs[j + 1] - xs[j]) * w;

			// construct rectangle
			ofRectangle rect = ofRectangle(cx, cy, cw, ch);

			// check if this rectangle is part of the floorplan
			bool contained = polygon.inside(rect.getCenter());

			// add cell to grid
			cells.push_back(GridCell(rect, contained));
		}
	}

	grid = FloorGrid(rows, cols, cells);
}
//--------------------------------------------------------------
void DebugState::update()
{

}

//--------------------------------------------------------------
void DebugState::draw()
{
	//ofClear()
	ofSetLineWidth(2);
	ofBackground(200);

	ofPushMatrix();
	{
		//ofScale(8,8);
		ofTranslate(20, 20);

		
	////	ofDrawCircle(rays[1].v2, 3);

	//	ofSetColor(20);
	//	ofPoint intersection;
	//	//if (IntersectionHelper::intersectRayLineSegment(
	//	//	rays[0].v1, rays[0].v2 - rays[0].v1, 
	//	//	rays[1].v1, rays[1].v2, 
	//	//	&intersection))
	//	//{
	//	//	ofDrawCircle(intersection, 4);
	//	//}

	//	//ofPolyline left;
	//	//ofPolyline right;
	//	//IntersectionHelper::splitPolygon(polygon, rays[0].v1, rays[0].v2 - rays[0].v1, &left, &right);
	//	
	//	for (int i = 0; i < grid.cells.size(); i++)
	//	{
	//		ofFill();
	//		ofSetColor(200);

	//		if (grid.cells[i].inside)
	//			ofSetColor(120);

	//		ofRect(grid.cells[i].rect);

	//		ofNoFill();
	//		ofSetColor(0, 200, 200);
	//		ofRect(grid.cells[i].rect);
	//	}

	//	ofFill();
	//	ofSetColor(200,0,0);
	//	ofRect(grid.getCellAt(2, 0).rect);

		ofNoFill();

		ofSetColor(20);


		polygon.draw();

	//	ofSetColor(0, 0, 255);
	//	ofSetLineWidth(2.0f);
	//	for (size_t i = 0; i < rays.size(); i++)
	//	{
	//		ofDrawLine(rays[i].v1, rays[i].v2);
	//		ofDrawCircle(rays[i].v1, 3);
	//	}


		//ofSetColor(0, 0, 255);
		//right.draw();

		//ofPolyline lleft;
		//ofPolyline lright;
		//IntersectionHelper::splitPolygon(left, rays[1].v1, rays[1].v2 - rays[1].v1, &lleft, &lright);

		//ofSetColor(255, 0, 0);
		//lleft.draw();

		//ofSetColor(0, 255, 0);
		//lright.draw();

		//ofSetColor(20);

		////ofDrawBitmapString(ofToString(left.getArea()), left.getCentroid2D());
		//ofDrawBitmapString(ofToString(right.getArea()), right.getCentroid2D());
		//ofDrawBitmapString(ofToString(lleft.getArea()), lleft.getCentroid2D());
		//ofDrawBitmapString(ofToString(lright.getArea()), lright.getCentroid2D());

		// generate straight skeleton
		vector<LineSegment> arcs;

		SSAlgOutput straightSkeleton = StraightSkeleton::CreateSkeleton(polygon, steps);

		// unpack tuple
		std::tie(arcs, faces) = straightSkeleton;

		skeleton = arcs;

		ofSetLineWidth(2);
		ofSetColor(20, 20, 200);
		ofNoFill();

		for (size_t i = 0; i < skeleton.size(); i++)
		{
			ofLine(skeleton[i].v1.x, skeleton[i].v1.y, skeleton[i].v2.x, skeleton[i].v2.y);
		}

		ofSetColor(200, 40, 40);
		ofFill();
		//for (size_t i = 0; i < faces.size(); i++)
		//{
		//	faces[3].draw();
		//}

		
		//faces[2].draw();
		//faces[4].draw();
		//faces[0].draw();
		//faces[3].draw();

		//ofNoFill();
	}
	ofPopMatrix();
}

//--------------------------------------------------------------
string DebugState::getName()
{
	return DebugState_StateName;
}

//--------------------------------------------------------------
void DebugState::keyPressed(int key)
{
	if (key == '[')
		steps = ofClamp(steps - 1, 0, 500);

	if (key == ']')
		steps = ofClamp(steps + 1, 0, 500);
}

//--------------------------------------------------------------
void DebugState::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void DebugState::mouseDragged(int x, int y, int button)
{
	//ofPoint mousePos = ofPoint(x, y);
	////// drag vertices
	////for (size_t i = 0; i < polygon.size(); i++)
	////{
	////	if (polygon[i].distance(mousePos) < 15.0f)
	////	{
	////		polygon[i] = mousePos;
	////		break;
	////	}
	////}


	//for (size_t i = 0; i < rays.size(); i++)
	//{
	//	if (rays[i].v1.distance(mousePos) < 20.0f)
	//	{
	//		rays[i].v1 = mousePos;
	//		break;
	//	}

	//	if (rays[i].v2.distance(mousePos) < 20.0f)
	//	{
	//		rays[i].v2 = mousePos;
	//		break;
	//	}
	//}
}