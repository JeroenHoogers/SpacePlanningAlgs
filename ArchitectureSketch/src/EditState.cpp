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
	interiorEvolver.generate(vector<int>());

	Genotype<float> g = interiorEvolver.selectionAlgorithm.population[0];
	walls = interiorEvolver.wallPlacementAlgorithm.population[0].genes;

	// manually edit gene based on dragged centers
	centers.clear();

	// add centers based on interior gene
	for (int i = 0; i < g.genes.size(); i+=2)
	{
		centers.push_back(floortoSceen(ofPoint(g.genes[i], g.genes[i + 1])));
	}

	interiorEvolver.optimizeInterior(0);
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
	interiorEvolver = BFSInteriorEvolver();
	interiorEvolver.setup(1, pProgram);

	interiorEvolver.generate(vector<int>());

	pFloor = &interiorEvolver.floors[0];

	centers.push_back(ofPoint(350, 400));
	centers.push_back(ofPoint(350, 200));
	centers.push_back(ofPoint(750, 400));
	centers.push_back(ofPoint(850, 200));
	centers.push_back(ofPoint(200, 500));

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


		if (pFloor != NULL)
		{
			// draw rooms
			for (int i = 0; i < pFloor->cells.size(); i++)
			{
				if (pFloor->cells[i].inside)
				{
					ofFill();

					//ofSetColor(255, (pFloor->cells[i].roomId) * 50, (pFloor->cells[i].roomId) * 50);

					ofSetColor(MeshHelper::getColor(pFloor->cells[i].roomId));
					ofRect(pFloor->cells[i].rect);
				}

				ofNoFill();
				//ofSetColor(50,50,255);
				//ofRect(pFloor->cells[i].rect);
			}

			// draw centroids
			for (int i = 0; i < pFloor->centers.size(); i++)
			{
				//ofSetColor(100, 100, 100);
				//	ofDrawLine(pFloor->centers[i], pFloor->centroids[i]);

				ofFill();

				//ofRectangle roomCell = pFloor->getCellAt(pFloor->centers[i].x, pFloor->centers[i].y).rect;
				ofSetColor(50, 50, 255);
				ofDrawCircle(pFloor->centers[i], 0.1f);

				ofSetColor(0, 0, 0);

				float area = pFloor->areas[i];

				//// sum the area of all rectangles
				//for (int j = 0; j < pFloor->cells.size(); j++)
				//{
				//	if (pFloor->cells[j].inside && pFloor->cells[j].roomId == i)
				//		area += pFloor->cells[j].rect.getArea();
				//}

				area = roundf(area * 10) / 10;
				ofPoint p = pFloor->centers[i] + ofPoint(5, -5) / ratio;
				ofDrawBitmapString(pProgram->rooms[i].code + "\n" + ofToString(area) + " m2", p);
			}

			// draw grid lines
			ofNoFill();
			ofSetLineWidth(1.0f);
			ofSetColor(150);

			for (auto& cell : pFloor->cells)
			{
				if (cell.inside)
				{
					ofRect(cell.rect);
				}
			}

			// draw walls
			ofSetLineWidth(1.5f);
			ofSetColor(0);

			int cells = pFloor->cells.size();
			for (int i = 0; i < walls.size(); i++)
			{
				int j = i % cells;

				if (!walls[i] || !pFloor->cells[j].inside)
					continue;

				ofRectangle& rect = pFloor->cells[j].rect;

				if (i < cells)
				{
					// right wall
					ofDrawLine(
						rect.getMaxX(), rect.getMinY(), 
						rect.getMaxX(), rect.getMaxY());
				}
				else
				{
					// bottom wall
					ofDrawLine(
						rect.getMinX(), rect.getMaxY(),
						rect.getMaxX(), rect.getMaxY());
				}
			}
		}

		// TODO: draw rooms
	}
	ofPopMatrix();

	// draw centers
	for (auto& center : centers)
	{
		ofFill();

		ofSetColor(20, 100, 100);
		ofDrawCircle(center, 5.0f);
	}
}

//--------------------------------------------------------------
string EditState::getName()
{
	return EditState_StateName;
}

//--------------------------------------------------------------
ofPoint EditState::screenToFloor(ofPoint screenPos)
{
	// convert screen position to x,y interpolation over floor bounding box
	ofRectangle bb = interiorEvolver.floorshape.getBoundingBox();

	// get lot rectangle
	ofRectangle lot = pProgram->getLotRectangle();

	// calculate ratio
	float ratioW = ofGetWidth() / lot.width;
	float ratioH = ofGetHeight() / lot.height;

	float ratio = fminf(ratioW, ratioH);

	ofMatrix4x4 t = ofMatrix4x4::newTranslationMatrix(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f, 0);
	ofMatrix4x4 s = ofMatrix4x4::newScaleMatrix(ratio, ratio, 1);
	ofMatrix4x4 transform =  s * t;

	ofRectangle bbScreen = ofRectangle(bb.getMin() * transform, bb.getMax() * transform);

	float x = (screenPos.x - bbScreen.getMinX()) / bbScreen.getWidth();
	float y = (screenPos.y - bbScreen.getMinY()) / bbScreen.getHeight();
	x = ofClamp(x, 0, 1);
	y = ofClamp(y, 0, 1);

	// compute top left of bb
	return ofPoint(x, y);
}

//--------------------------------------------------------------
ofPoint EditState::floortoSceen(ofPoint floorPos)
{
	// convert screen position to x,y interpolation over floor bounding box
	ofRectangle bb = interiorEvolver.floorshape.getBoundingBox();

	// get lot rectangle
	ofRectangle lot = pProgram->getLotRectangle();

	// calculate ratio
	float ratioW = ofGetWidth() / lot.width;
	float ratioH = ofGetHeight() / lot.height;

	float ratio = fminf(ratioW, ratioH);

	ofMatrix4x4 t = ofMatrix4x4::newTranslationMatrix(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f, 0);
	ofMatrix4x4 s = ofMatrix4x4::newScaleMatrix(ratio, ratio, 1);
	ofMatrix4x4 transform = s * t;

	ofRectangle bbScreen = ofRectangle(bb.getMin() * transform, bb.getMax() * transform);

	float x = bbScreen.getMinX() + (floorPos.x * bbScreen.getWidth());
	float y = bbScreen.getMinY() + (floorPos.y * bbScreen.getHeight());

	// compute top left of bb
	return ofPoint(x, y);
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
	for (int i = 0; i < centers.size(); i++)
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
	Genotype<float> g = interiorEvolver.selectionAlgorithm.population[0];

	// manually edit gene based on dragged centers
	for (int i = 0; i < centers.size(); i++)
	{
		ofPoint p = screenToFloor(centers[i]);
		g.genes[i * 2] = p.x;
		g.genes[i * 2 + 1] = p.y;
	}

	interiorEvolver.selectionAlgorithm.population[0] = g;

	// When we are done dragging, optimize the room layout
	interiorEvolver.optimizeInterior(0);
}
