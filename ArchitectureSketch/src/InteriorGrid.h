#pragma once

#include "ofMain.h"
#include "ArchitectureProgram.h"

struct GridVertex
{
	int id;
	int x, y;

	GridVertex(int _id, int _x, int _y)
	{
		id = _id;
		x = _x;
		y = _y;
	}
};

struct GridGraph
{
	vector<GridVertex> vertices; 
	map<int, list<int>> adj; // adjacency lists indexed by vertex id

	// add edge between vertices v1 and v2
	void addEdge(int v1, int v2)
	{
		adj[v1].push_back(v2);
		adj[v2].push_back(v1);
	};
};

class InteriorGrid
{
private:
	int hCuts, vCuts;
	int totalCells;

public:

	//--------------------------------------------------------------
	InteriorGrid(int _hCuts, int _vCuts)
	{
		hCuts = _hCuts;
		vCuts = _vCuts;

		totalCells = (hCuts + 1) * (vCuts + 1);
	}

	//--------------------------------------------------------------
	int getCellIndex(int x, int y)
	{
		// computes the cell index given x, y values
		if (x > vCuts || y > hCuts)
			return -1;

		return y * (vCuts + 1) + x;
	}

	//--------------------------------------------------------------
	GridGraph GenerateGraph(vector<bool> walls)
	{
		GridGraph graph;

		// construct graph
		for (int i = 0; i <= hCuts; i++)
		{
			for (int j = 0; j <= vCuts; j++)
			{
				int u = getCellIndex(j, i);

				// add vertex
				graph.vertices.push_back(GridVertex(u, j, i));

				// create edge right
				if(j < vCuts )//&& !walls[u])
				{
					int v = getCellIndex(j + 1, i);
					if (v > 0)
						graph.addEdge(u, v);
				}

				// create edge down
				if (i < hCuts )// && !walls[u + totalCells])
				{
					int v = getCellIndex(j, i + 1);
					if(v > 0)
						graph.addEdge(u, v);
				}
			}
		}

		return graph;
	};

	ofPoint getCellInterpolated(ofPoint center)
	{
		// compute cell corresponding to the centerpoint
		int x = fminf(floorf(center.x * (vCuts + 1)), vCuts);
		int y = fminf(floorf(center.y * (hCuts + 1)), hCuts);

		return ofPoint(x, y);
	}

	//--------------------------------------------------------------
	vector<map<int, int>> GenerateRooms(GridGraph& graph, const vector<Room>& rooms, const vector<ofPoint>& centers)
	{
		vector<map<int, int>> roomLevels;

		// sanity check
		if(rooms.size() != centers.size())
			return roomLevels;

		for (int i = 0; i < rooms.size(); i++)
		{
			// compute cell corresponding to the centerpoint
			//int x = fminf(floorf(centers[i].x * (vCuts + 1)), vCuts);
			//int y = fminf(floorf(centers[i].y * (hCuts + 1)), hCuts);
			ofPoint cell = getCellInterpolated(centers[i]);
			int center = getCellIndex((int)cell.x, (int)cell.y);

			roomLevels.push_back(BreathFirstSearch(graph, center));
		}

		return roomLevels;
	}

	//--------------------------------------------------------------
	map<int,int> BreathFirstSearch(GridGraph& graph, int s)
	{
		map<int, int> level = { {s, 0} };		// stores the depth of each vertex compared to the starting vertex
		vector<int> frontier = { s };			// stores the current frontier vertices
		int i = 1;

		while (frontier.size() > 0)
		{
			vector<int> next;

			// go over the vertices in the current frontier
			for (auto& u : frontier)
			{
				// go over all vertices adjacent to the current frontier vertex
				for (auto& v : graph.adj[u])
				{
					// check whether this vertex is already in our level set, if not add it
					map<int, int>::iterator it = level.find(v);
					if (it == level.end())
					{
						level[v] = i;
						next.push_back(v);
					}
				}
			}

			frontier = next;
			i++;
		}

		// return level map (no level means unreachable)
		return level;
	}
};