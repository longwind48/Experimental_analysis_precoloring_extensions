#include "stdafx.h"
#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <iomanip>
#include <algorithm>
#include <string>
using namespace std;

// Before a precolored graph can be put into a graph coloring algorithm, this function calls precoloring converter for fixed k
void preStepsGCP(int &numNodes, int &numEdges, vector<int> &numNodesIndepSet, int &numPrecolorsChosen, int &numColorsChosen, int &many) {
	
	int orig_numColorsChosen = numColorsChosen;

	// for loop to generate -n files 
	for (int i = 0; i < many+1; i++) {
		// Open newgraph.txt and copy its contents to newnewgraph.txt
		ifstream in1;
		in1.open("newgraph.txt");

		numColorsChosen = orig_numColorsChosen + i;
		ofstream newStrm;
		newStrm.open("newnewgraph"+to_string(numColorsChosen) + ".txt");
		newStrm << "c A Graph Transformation of Precoloring Extensions to run on ordinary Graph Coloring Algorithms\n"
			<< "c \n"
			<< "c Initial input graph has [" << numNodes << "] nodes and [" << numEdges << "] edges.\n"
			<< "c [" << numColorsChosen << "] number of colors has been selected to test.\n"
			//<< "c A total of [" << numNodesIndepSet.size() << "] nodes has been precolored with [" << numPrecolorsChosen << "] distinct number of colors.\n"
			<< "c A total of [" << numNodesIndepSet.size() << "] nodes has been precolored with [" << numPrecolorsChosen << "] distinct number of colors.\n"
			<< "c Your input graph has been precolored and transformed.\n"
			<< "c It can now be run as an ordinary Graph Coloring Problem.\n"
			<< "c This graph is in DIMACS format.\n"
			<< "c *********************************************************************************\n";

		newStrm << "p edge " << numNodes + numColorsChosen << ' '
			<< numEdges + ((numNodes + numColorsChosen)*(numNodes + numColorsChosen - 1) / 2) << "\n";

		// Specify edges for K_k
		for (int d = numNodes + 1; d < numNodes + numColorsChosen + 1; d++) {
			for (int f = numNodes + 1; f < numNodes + numColorsChosen + 1; f++) {
				if (f != d) {
					newStrm << "e " << d << ' ' << f << "\n";
				}
				//for (int m = g.n - g.numPrecoloredNodes + 1; m < g.n + 1; m++)
				//	newStrm << "e " << d << ' ' << m << "\n";
			}
		}
		for (int i = 0; i < numNodesIndepSet.size(); i++) {
			for (int s = numNodes + 1 + numPrecolorsChosen; s < numNodes + numColorsChosen + 1; s++) {
				newStrm << "e " << numNodesIndepSet[i] + 1 << ' ' << s << "\n";
			}
		}

		// Combine the K_k and new edges
		std::string line;
		while (std::getline(in1, line))
		{
			if (line.find('e') == 0)
				// copy line to output
				newStrm << line << '\n';
			//continue;
		}

		in1.close();
		newStrm.close();
	}
	
}
