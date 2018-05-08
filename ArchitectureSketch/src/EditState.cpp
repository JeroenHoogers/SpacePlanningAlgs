#include "EditState.h"

//--------------------------------------------------------------
EditState::EditState()
{
}

//--------------------------------------------------------------
EditState::~EditState()
{
}

//--------------------------------------------------------------
void EditState::stateEnter()
{
	// retrieve floor shape from shared data
	ofPolyline floorshape = getSharedData().floorshape;

	interiorEvolver.setFloorShape(floorshape);
}

//--------------------------------------------------------------
void EditState::stateExit()
{

}


//--------------------------------------------------------------
void EditState::setup()
{
	// get architectural program
	pProgram = &getSharedData().program;

	// create interior evolver
	interiorEvolver = InteriorEvolver();
	interiorEvolver.setup(1, pProgram);

	//polygon = ofPolyline();
	//polygon.addVertex(ofPoint(20, 20));
	//polygon.addVertex(ofPoint(350, 20));
	//polygon.addVertex(ofPoint(350, 400));
	//polygon.addVertex(ofPoint(750, 400));
	//polygon.addVertex(ofPoint(750, 700));
	//polygon.addVertex(ofPoint(200, 700));
	//polygon.addVertex(ofPoint(200, 500));
	//polygon.addVertex(ofPoint(20, 500));
	//polygon.close();

}

//--------------------------------------------------------------
void EditState::update()
{

}

//--------------------------------------------------------------
void EditState::draw()
{
	//ofClear()
	ofSetLineWidth(2);
	ofBackground(200);

	// get lot rectangle
	ofRectangle lot = pProgram->getLotRectangle();

	// calculate ratio
	float ratioW = ofGetWidth() / lot.width;
	float ratioH = ofGetHeight() / lot.height;

	float ratio = fminf(ratioW, ratioH);
	//ratio *= camera.getDistance();

	ofPushMatrix();
	{
		ofNoFill();
		ofSetColor(0);

		ofTranslate(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f);
		ofScale(ratio, ratio);

		// draw lot
		ofSetLineWidth(1.0f);
		ofRect(lot);

		// draw floor outline
		ofSetLineWidth(3.0f);
		interiorEvolver.floorshape.draw();

		// draw rooms
		ofSetColor(40);
		ofSetLineWidth(1.5f);

		//if (pInterior != NULL)
		//{
		//	for (int i = 0; i < pInterior->size(); i++)
		//	{
		//		ofFill();
		//		
		//		(*pInterior)[i].shape.draw();
		//		
		//		// draw room name and area
		//		float area = roundf((*pInterior)[i].getArea() * 10) / 10;
		//		ofPoint p = (*pInterior)[i].shape.getCentroid2D() + ofPoint(-5, -10) / ratio;
		//		ofDrawBitmapString((*pInterior)[i].pRoom->code + "\n" + ofToString(area) + " m2", p);
		//	}
		//}

		//if (pFloor != NULL)
		//{
		//	for (int i = 0; i < pFloor->cells.size(); i++)
		//	{
		//		if (pFloor->cells[i].inside)
		//		{
		//			ofFill();

		//			//ofSetColor(255, (pFloor->cells[i].roomId) * 50, (pFloor->cells[i].roomId) * 50);

		//			ofSetColor(MeshHelper::getColor(pFloor->cells[i].roomId));
		//			ofRect(pFloor->cells[i].rect);
		//		}

		//		ofNoFill();
		//		//ofSetColor(50,50,255);
		//		//ofRect(pFloor->cells[i].rect);
		//	}

		//	for (int i = 0; i < pFloor->centers.size(); i++)
		//	{
		//		//ofSetColor(100, 100, 100);
		//		//	ofDrawLine(pFloor->centers[i], pFloor->centroids[i]);

		//		ofFill();

		//		//ofRectangle roomCell = pFloor->getCellAt(pFloor->centers[i].x, pFloor->centers[i].y).rect;
		//		ofSetColor(50, 50, 255);
		//		ofDrawCircle(pFloor->centers[i], 0.1f);


		//		ofSetColor(0, 0, 0);

		//		float area = pFloor->areas[i];

		//		//// sum the area of all rectangles
		//		//for (int j = 0; j < pFloor->cells.size(); j++)
		//		//{
		//		//	if (pFloor->cells[j].inside && pFloor->cells[j].roomId == i)
		//		//		area += pFloor->cells[j].rect.getArea();
		//		//}

		//		area = roundf(area * 10) / 10;
		//		ofPoint p = pFloor->centers[i] + ofPoint(5, -5) / ratio;
		//		ofDrawBitmapString(pProgram->rooms[i].code + "\n" + ofToString(area) + " m2", p);
		//	}
		//}

		// TODO: draw rooms
	}
	ofPopMatrix();
}

//--------------------------------------------------------------
string EditState::getName()
{
	return EditState_StateName;
}

//--------------------------------------------------------------
void EditState::keyPressed(int key)
{
	if (key == '[')
		steps = ofClamp(steps - 1, 0, 500);

	if (key == ']')
		steps = ofClamp(steps + 1, 0, 500);
}

//--------------------------------------------------------------
void EditState::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void EditState::mouseDragged(int x, int y, int button)
{
	ofPoint mousePos = ofPoint(x, y);
	//// drag vertices
	//for (size_t i = 0; i < polygon.size(); i++)
	//{
	//	if (polygon[i].distance(mousePos) < 15.0f)
	//	{
	//		polygon[i] = mousePos;
	//		break;
	//	}
	//}


	// drag points
	for (size_t i = 0; i < centers.size(); i++)
	{
		if (centers[i].distance(mousePos) < 20.0f)
		{
			centers[i] = mousePos;
			break;
		}
	}
}

//--------------------------------------------------------------
void EditState::mouseReleased(int x, int y, int button)
{
	// When we are done dragging, optimize the room layout
	interiorEvolver.optimizeInterior(0);
}
