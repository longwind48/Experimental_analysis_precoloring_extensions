#ifndef GraphIncluded
#define GraphIncluded
#include <vector>

class Graph {
public:

	Graph();
	Graph(int n);
	~Graph();
	//  Graph(char * file);

	void resize(int n);

	int *matrix;
	int n;        // number of nodes
	int nbEdges;  // number of edges
	int numPrecoloredNodes; // number of precolored nodes
	std::vector<int> precoloredNodes;

	int * operator[](int index);
};

#endif
