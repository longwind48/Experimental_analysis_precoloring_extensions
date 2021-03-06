/******************************************************************************/
//  This program generates a random graph for a specified number of vertices and density:
//
//  
//	To run the program, no command arguments are needed.
//  --------------------------------------------------
/******************************************************************************/

#include "stdafx.h"
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;

double prob() {
	return (static_cast<double>(rand()) / RAND_MAX);
}

int main()
{
	int edgeCount = 0;
	int size = 15; 
	double density;
	
	cout << "graph size?" << endl;
	cin >> size;
	cout << "graph density (0,1)?" << endl;
	cin >> density;

	bool** graph;
	int** color;
	int** cost;
	srand(2); //seed random number generator

	// Construct the different components, basically 3 matrices
	graph = new bool*[size];
	color = new int*[size];
	cost = new int*[size];
	for (int i = 0; i < size; i++) {
		graph[i] = new bool[size];
		color[i] = new int[size];
		cost[i] = new int[size];
	}

	// Generate undirected edges
	for (int i = 0; i < size; ++i) {
		for (int j = i; j < size; ++j) {
			if (i == j) {
				graph[i][j] = false; //no loops
			}
			else { 
				graph[i][j] = graph[j][i] = (prob() < density); 
			}
		}
	}
	// Generate costs and colors
	for (int i = 0; i < size; ++i) {
		for (int j = i; j < size; ++j) {
			if (graph[i][j]) {
				color[i][j] = color[j][i] = rand() % 3;
				cost[i][j] = cost[j][i] = prob() * 30;

			}
		}
	}
	// Output graph is saved in graph.txt
	ofstream outp("graph.txt");
	for (int i = 0; i < size; ++i) {
		for (int j = i; j < size; ++j) {
			if (graph[i][j]==true) {
				edgeCount++;
			}
		}
	}
	// display the number of vertices and edges in graph.txt
	outp << "p edge "<< size << ' '<< edgeCount << "\n";
	for (int i = 0; i < size; ++i) {
		for (int j = i; j < size; ++j) {
			if (graph[i][j] == true) {
				outp << "e " << i + 1 << ' ' << j + 1 << "\n";
			}
		}
	}

    return 0;
}

