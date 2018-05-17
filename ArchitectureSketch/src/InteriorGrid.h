#pragma once

#include "ofMain.h"

struct GridVertex
{
	int id;
	int x;
	int y;

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
	int hCuts;
	int vCuts;

	public:
		InteriorGrid(int _hCuts, int _vCuts)
		{
			hCuts = _hCuts;
			vCuts = _vCuts;
		}

		GridGraph GenerateGraph(vector<bool> walls)
		{
			GridGraph graph;

			vector<GridVertex> verts;

			// construct vertices
			for (int i = 0; i <= hCuts; i++)
			{
				for (int j = 0; j <= vCuts; j++)
				{
					int index = i * vCuts + j;
					verts.push_back(GridVertex(index, j, i));
				}
			}

			graph.vertices = verts;

			// add edges
			for (int i = 0; i <= hCuts; i++)
			{
				for (int j = 0; j <= vCuts; j++)
				{
					// if no wall exists, add an edge
					int index = i * vCuts + j;
					//if(!walls[index])
					{
						// compute i2
						int index2 = i * vCuts + j + 1;
					}

					verts[index]
				}
			}
		};

};