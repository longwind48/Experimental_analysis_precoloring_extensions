/******************************************************************************/
//  This code precolors any graph in two steps:
//	1) Find the maximal independent set with minimum-degree-greedy over 10 different seeds.
//	2) Randomly assign a color from a specified number of colors to each vertex in the independent set.
//
//  The code was written by Traci Lim, the code skeleton was adapted from the code package from 
//	R.M.R Lewis's book: A Guide to Graph Colouring: Algorithms and Applications,
//	which was taken from rhydlewis.eu/resources/gCol.zip
//  
//	To run the program, input the following example command.
//  --------------------------------------------------
//	"graph-1000-10.txt -r 1 -v -v -p 1 -c 23 -n 10"
//	--------------------------------------------------
//	"graph-1000-10.txt" is a graph in DIMACS format
//	"-r 1" is the random seed set as 1
//	"-v -v" sets verbose to true, which shows more information
//	"-p 1" sets the number of unique colors in precoloring to 1
//	"-c 23" sets k=23, where k is the k-coloring to be fixed 
//	"-n 10" outputs ten converted precolored graphs with 10 consecutive levels of k
/******************************************************************************/
#include "stdafx.h"
#include "PreGCPFixedKTransformation.h"
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

unsigned long long numConfChecks;

//-------------------------------------------------------------------------------------
void swap(int &a, int &b) {
	int x = a; a = b; b = x;
}

//-------------------------------------------------------------------------------------
void readInputFile(ifstream &inStream, int &numNodes, int &numEdges, vector< vector<bool> > &adjacent, vector<int> &degree, vector< vector<int> > &adjList)
{
	//Reads a DIMACS format file and creates the corresponding degree array and adjacency matrix
	char c;
	char str[1000]; // char array size 1000
	int line = 0, i, j;
	numEdges = 0;
	int edges = -1;
	int blem = 1;
	int multiple = 0;
	while (!inStream.eof()) {
		line++;
		inStream.get(c);
		if (inStream.eof()) break;
		switch (c) {
		case 'p': // if first char == 'p'
			inStream.get(c);
			inStream.getline(str, 999, ' '); //max # char to write to char array str is 999, operation of extracting succesive characters stops as soon as next char is whitespace

			// compare str and string "edge" character by character; returns 0 (false) if str contains "edge", 1 (true) otherwise
			if (strcmp(str, "edge") && strcmp(str, "edges")) {
				cerr << "Error reading 'p' line: no 'edge' keyword found.\n";
				cerr << "'" << str << "' found instead\n";
				exit(-1);
			}

			inStream >> numNodes;
			inStream >> numEdges;

			//Set up the 2d adjacency matrix
			adjacent.clear(); //Removes all elements from the vector (which are destroyed), leaving the container with a size of 0.

			// new container size, number of elements = numNodes
			// new elements are initialized with a bool vector of size numNodes
			adjacent.resize(numNodes, vector<bool>(numNodes));

			for (i = 0;i<numNodes;i++) for (j = 0;j<numNodes;j++) {
				if (i == j)adjacent[i][j] = true;
				else adjacent[i][j] = false; // if i != j, then give it 0. 
			}
			blem = 0;
			break;
		case 'n':
			if (blem) {
				cerr << "Found 'n' line before a 'p' line.\n";
				exit(-1);
			}
			int node;
			inStream >> node;
			if (node < 1 || node > numNodes) {
				cerr << "Node number " << node << " is out of range!\n";
				exit(-1);
			}
			node--;
			cout << "Tags (n Lines) not implemented in g object\n";
			break;
		case 'e':
			int node1, node2;
			inStream >> node1 >> node2;

			//if node index is out of range
			if (node1 < 1 || node1 > numNodes || node2 < 1 || node2 > numNodes) { // 
				cerr << "Node " << node1 << " or " << node2 << " is out of range!\n";
				exit(-1);
			}
			node1--;
			node2--;

			// if node1 and node2 has no edge recorded in adj matrix, increase edges count
			if (!adjacent[node1][node2]) {
				edges++;
			}
			else { // if node1 and node2 already has an edge recorded, increase multiple count
				multiple++;
				if (multiple<5) {
					cerr << "Warning: in graph file at line " << line << ": edge is defined more than once.\n";
					if (multiple == 4) {
						cerr << "  No more multiple edge warnings will be issued\n";
					}
				}
			}

			// record an edge between node1 and node2 in the adj matrix
			adjacent[node1][node2] = true;
			adjacent[node2][node1] = true;
			break;
		case 'd':
		case 'v':
		case 'x':
			cerr << "File line " << line << ":\n";
			cerr << "'" << c << "' lines are not implemented yet...\n";
			inStream.getline(str, 999, '\n');
			break;
		case 'c':
			inStream.putback('c'); //Attempts to decrease the current location in the stream by one character, making the last character extracted from the stream once again available to be extracted by input operations.
			inStream.get(str, 999, '\n');
			break;
		default:
			cerr << "File line " << line << ":\n";
			cerr << "'" << c << "' is an unknown line code\n";
			exit(-1);
		}
		inStream.get(); // Kill the newline; //Extracts a single character from the stream.
	}
	inStream.close();
	if (multiple) {
		cerr << multiple << " multiple edges encountered\n";
	}
	//Now use the adjacency matrix to construct the degree array and adjacency list
	degree.resize(numNodes, 0); //degree is a vector, initialize vector degree with all 0's
	adjList.resize(numNodes, vector<int>()); //2d int vector
	for (i = 0; i<numNodes; i++) {
		for (j = 0; j<numNodes; j++) {
			if (adjacent[i][j] && i != j) {
				adjList[i].push_back(j); //at the ith index of adjList vector, add index j to it. So for each node (row), it will contain the indices of all nodes it's adjacent to.
				degree[i]++; // increase the degree of node i by 1
			}
		}
	}
}

//-------------------------------------------------------------------------------------
inline
void removeElement(vector<int> &A, int i) {
	//Constant time operation for removing an item from a vector (note that ordering is not maintained)
	swap(A[i], A.back()); //swap the element you want to delete with the reference to the last element
	A.pop_back(); //delete last element
}

inline
void chooseMinDegVertex(vector<int> &X, int &v, int &vPos, vector<int> &Deg) {
	//Select the vertex in X that has the min corresponding value in vector Deg (aka minimum degree)
	int i, min = Deg[0], numMin = 1;
	v = X[0]; // choose a arbitrary vertex and degree
	vPos = 0;

	// Go through the all nodes in vertex vector, take the index of the vertex with smallest degree, 
	for (i = 1;i<X.size();i++) {
		if (Deg[i] <= min) {
			if (Deg[i]<min) numMin = 0; // once a vertex with degree less than current min is spotted, set numMin to 0
			if (rand() % (numMin + 1) == 0) { // use rand() to generate a random selection of vertices with same minimal degree
				min = Deg[i]; 
				v = X[i]; 
				vPos = i; 
			}
			numMin++;
		}
	}
}
//-------------------------------------------------------------------------------------
inline
void updateX(vector<int> &X, vector<int> &XDeg, int v, int vPos, vector<int> &Y, vector<int> &YDeg, vector< vector<bool> > &adjacent, vector<int> &NInY) {
	int i = 0, j;
	//Remove v from X and update the relevant vectors XDeg, NInY
	removeElement(X, vPos);
	removeElement(XDeg, vPos);
	removeElement(NInY, vPos);
	//Transfer all vertices in X that are adjacent to v (which has already been removed) into Y. Also update the degree vectors
	while (i<X.size()) {
		numConfChecks++;
		if (adjacent[X[i]][v]) { //for any vertex adjacent to v
			// Move vertex at X[i] to Y. 
			// Also transfer X[i]'s degree minus 1 (because the edge between v and X[i] has been removed)
			Y.push_back(X[i]); // add it to vector Y
			YDeg.push_back(XDeg[i] - 1);
			removeElement(X, i);
			removeElement(XDeg, i);
			removeElement(NInY, i);

			//Since a new vertex is being moved to Y, any vertex in X that is adjacent has its NInY entry updated
			for (j = 0;j<X.size();j++) {
				numConfChecks++;
				if (adjacent[X[j]][Y.back()]) {
					NInY[j]++;
				}
			}
		}
		else {
			i++;
		}
	}
}
inline
void removeDuplicateColors(vector< vector<int> > &candSol, vector<int> &colNode) {

	// We need a replica of candSol vector because we will be removing elements from candSol, and we want to keep the indices unchanged
	vector< vector<int> > candSolTemp;
	candSolTemp = candSol;

	// loop through all color classes 1 onwards, if identical color with color class 0 exists, remove node from color class 0.
	for (int i = 1; i<candSol[0].size(); i++) {
		for (int group = 1; group<candSol.size(); group++) {
			for (int h = 0; h<candSol[group].size(); h++) {
				if (candSol[group][h] == candSol[0][i]) {

					// Finding an element in vector 
					vector<int>::iterator it = std::find(candSolTemp[0].begin(), candSolTemp[0].end(), candSolTemp[group][h]);

					// Get index of element from iterator
					int index = distance(candSolTemp[0].begin(), it);

					removeElement(candSolTemp[0], index);
					//colNode[candSol[group][h]] = INT_MIN;
				}
			}
		}
	}
	candSol = candSolTemp;
}


// This function randomly recolor the precolored vertices to numColorsChosen different colors 
// It makes sure that if numColorsChosen is equal to number of precolored nodes, every node gets a distinct color
inline
void precolor(vector< vector<int> > &candSol, vector<int> &colNode, int numColorsChosen)
{
	// Open up new colors
	// start from 1 since color 0 is already opened
	for (int p = 1; p < numColorsChosen; p++) {
		candSol.push_back(vector<int>());
	}

	// If numColorsChosen is more/equal than number of nodes assigned color 0
	if (numColorsChosen >= candSol[0].size()) {
		// Go through all vertices colored 0, 
		for (int q = 1; q < candSol[0].size(); q++) {
			candSol[q].push_back(candSol[0][q]);
			colNode[candSol[0][q]] = q;
			//candSol[0][q] = INT_MIN;
		}
		removeDuplicateColors(candSol, colNode);

	}
	// if numColorsChosen < candSol[0].size(), remaining vertices must be randomly assigned a color
	else {
		// Assign the first few vertices with distinct colors until all colors are used up, 
		// We do this to avoid all vertices being randomly assigned the same color.
		for (int q = 1; q < numColorsChosen; q++) {
			candSol[q].push_back(candSol[0][q]);
			colNode[candSol[0][q]] = q;
		}
		// Remaining vertices are randomly assigned a color.
		for (int r = numColorsChosen; r < candSol[0].size(); r++) {
			int randColor = rand() % (numColorsChosen);

			if (randColor != 0) {
				candSol[randColor].push_back(candSol[0][r]);
				colNode[candSol[0][r]] = randColor;
			}
		}
		removeDuplicateColors(candSol, colNode);
	}
}



//-------------------------------------------------------------------------------------
inline
void makeSolution(vector< vector<int> > &candSol, int verbose, vector<int> &degree, vector< vector<int> > &adjList, vector<int> &colNode, vector< vector<bool> > &adjacent, int numNodes, vector<int> &numNodesIndepSet)
{
	int i, c, v, vPos;
	candSol.clear(); // 2d vector size is now 0

	//X is a vector containing all unplaced vertices that can go into the current colour c (initially contains all unplaced vertices).
	//XDeg contains the degree of the vertices induced by the subgraph of X (i.e. XDeg[i] == degree of X[i])
	//Y is used to hold vertices that clash with vertices currently assigned to colour c (initially empty).
	//YDeg contains the degree of the vertices induced by the subgraph of Y (i.e. YDeg[i] == degree of Y[i])
	//NInY contains the number of neighbours vertex X[i] has in Y (initially zero because Y is empty at the start of each iteration)
	vector<int> X(numNodes), Y, XDeg, YDeg, NInY(numNodes, 0);
	for (i = 0;i<numNodes;i++) X[i] = i; //initialize vector X
	XDeg = degree;
	c = 0;

	// Run the generatePrecoloring algorithm
	// Two main steps: 
	// while X is not empty, 
	if (c == 0) { 
		//while (!X.empty()) {
		//Open a new colour c, by adding a new element(vector in this case) at the end of the vector
		candSol.push_back(vector<int>());

		//Choose the vertex v in X that has the largest degree in the subgraph induced by X, then add v to colour c
		//chooseVertex(X, v, vPos, XDeg); // this o/p v, the vertex with largest degree
		chooseMinDegVertex(X, v, vPos, XDeg);

		// Assign color c to vertex v
		candSol[c].push_back(v);
		colNode[v] = c;

		updateX(X, XDeg, v, vPos, Y, YDeg, adjacent, NInY);
		while (!X.empty()) {
			// Choose the vertex v that has the smallest degree, then remove v and its neighbors from X, repeat until X is empty
			chooseMinDegVertex(X, v, vPos, XDeg);
			candSol[c].push_back(v);
			colNode[v] = c;
			updateX(X, XDeg, v, vPos, Y, YDeg, adjacent, NInY);
		}
		X.swap(Y); // Since X is now empty, swap contents with Y
		XDeg.swap(YDeg);
		NInY.resize(X.size(), 0); //reset size vector NInY and set all 0's
								  //c++;
	}
	numNodesIndepSet = candSol[0];
}


//-------------------------------------------------------------------------------------
inline
void prettyPrintSolution(vector< vector<int> > &candSol, vector<int> &numNodesIndepSet, int &numNodes, int &numEdges, int &maximal, int &setseed)
{
	int i, count = 0, group;
	cout << "\n\n";
	for (group = 0; group<candSol.size(); group++) {
		cout << "C-" << group << "\t= {";
		if (candSol[group].size() == 0) cout << "empty}\n";
		else {
			for (i = 0; i<candSol[group].size() - 1; i++) {
				cout << candSol[group][i]+1 << ", ";
			}
			cout << candSol[group][candSol[group].size() - 1]+1 << "}\n";
			count = count + candSol[group].size();
		}
	}
	cout << "Total Number of Nodes and Edges = " << '(' << numNodes << ", " << numEdges << ')' << endl;
	cout << "Number of Nodes in maximal independent set = " << numNodesIndepSet.size() << endl;
	cout << "Precolored vertices have " << candSol.size() << " unique number of colors."<< endl;
	cout << "Maximum size of maximal independent set over 10 random seeds is " << maximal << ", with seed = "<< setseed << endl;
}

//-------------------------------------------------------------------------------------
inline
void checkSolution(vector< vector<int> > &candSol, vector< vector<bool> > &adjacent, int numNodes)
{
	int j, i, count = 0, group;
	bool valid = true;

	// Finally, check for illegal colourings: I.e. check that each colour class contains non conflicting nodes
	for (group = 0; group < candSol.size(); group++) {
		for (i = 0; i < candSol[group].size() - 1; i++) {
			for (j = i + 1; j < candSol[group].size(); j++) {
				if (adjacent[candSol[group][i]][candSol[group][j]]) {
					cout << "Error: Nodes " << candSol[group][i] << " and " << candSol[group][j] << " are in the same group, but they clash" << endl;
					valid = false;
				}
			}
		}
	}
	if (!valid) cout << "This solution is not valid" << endl;
}


//-------------------------------------------------------------------------------------
int main(int argc, char ** argv) {

	if (argc <= 1) {
		cout << "Generating precolored graph from uncolored graph\n\n"
			<< "USAGE:\n"
			<< "<InputFile>     (Required. File must be in DIMACS format)\n"
			<< "-r <int>        (Random seed. DEFAULT = 1)\n"
			<< "-v              (Verbosity. If present, output is sent to screen. If -v is repeated, more output is given.)\n"
			<< "-c <int>		(Number of distinct colors for precoloring. DEFAULT = 1)\n"
			<< "****\n";
		exit(1);
	}

	int i, verbose = 0, randomSeed = 1, numNodes, numEdges = 0, numPrecolorsChosen=0, numColorsChosen = 0, precoloringMode=0, prepareForGCP=1, many=1;
	vector<int> degree;
	vector< vector<int> > adjList;
	vector< vector<bool> > adjacent;
	numConfChecks = 0;

	for (i = 1; i < argc; i++) {
		if (strcmp("-r", argv[i]) == 0) {
			randomSeed = atoi(argv[++i]);
		}
		else if (strcmp("-v", argv[i]) == 0) {
			verbose++;
		}
		else if (strcmp("-p", argv[i]) == 0) {
			precoloringMode = 1;
			numPrecolorsChosen = atoi(argv[++i]);
		}
		else if (strcmp("-c", argv[i]) == 0) {
			prepareForGCP = 2;
			numColorsChosen = atoi(argv[++i]);
		}
		else if (strcmp("-n", argv[i]) == 0) {
			many = atoi(argv[++i]);
		}
		else {
			//Set up input file, read, and close (input must be in DIMACS format)
			ifstream inStream;
			inStream.open(argv[i]);

			// initialize adj matrix, adjList, degree vector, edges count, multiple count, numEdges, numNodes
			readInputFile(inStream, numNodes, numEdges, adjacent, degree, adjList);
			inStream.close();
		}
	}

	//Set Random Seed
	srand(randomSeed);

	//Check to see if there are no edges. If so, exit straight away
	if (numEdges <= 0) {
		if (verbose >= 1) cout << "Graph has no edges. Optimal solution is obviously using one colour. Exiting." << endl;
		exit(1);
	}

	//Start the timer
	clock_t runStart = clock();

	//Declare strucures used for holding the solution
	int maximal, setseed;
	vector< vector<int> > candSol;
	vector<int> colNode(numNodes, -1), numNodesIndepSet;

	// Over 10 seeds, find the seed that produce the maximum size of maximal independent set
	maximal = 0;
	for (int i = randomSeed; i < randomSeed+10; i++) {
		int randomnum = rand() % 10;
		srand(i+ randomnum);
		makeSolution(candSol, verbose, degree, adjList, colNode, adjacent, numNodes, numNodesIndepSet);
		if (numNodesIndepSet.size() >= maximal) {
			maximal = numNodesIndepSet.size();
			setseed = i+ randomnum;
		}
	}

	// Greedily find the maximal indepedent set and assign color 0 to all vertices
	srand(setseed);
	makeSolution(candSol, verbose, degree, adjList, colNode, adjacent, numNodes, numNodesIndepSet);

	// Precolor maximal independent set with numColorsChosen number of colors
	if (precoloringMode = 1) {
		precolor(candSol, colNode, numPrecolorsChosen);

		// Output the solution to a text file
		ofstream solStrm;
		solStrm.open("precolorSolution.txt");
		solStrm << numNodes << "\n";
		// from the first line, for each node, print the color class it was assigned to 
		for (i = 0;i < numNodes;i++) solStrm << i + 1 << ' ' << colNode[i] << "\n";
		solStrm.close();
	}

	//Stop the timer.
	clock_t runFinish = clock();
	int duration = (int)(((runFinish - runStart) / double(CLOCKS_PER_SEC)) * 1000);

	if (verbose >= 1) cout << " COLS     CPU-TIME(ms)\tCHECKS" << endl;
	if (verbose >= 1) cout << setw(5) << candSol.size() << setw(11) << duration << "ms\t" << numConfChecks << endl;
	if (verbose >= 2) {
		prettyPrintSolution(candSol, numNodesIndepSet, numNodes, numEdges, maximal, setseed);
		checkSolution(candSol, adjacent, numNodes);
	}


	// Create a new graph from the precolored vertices by adding a complete graph K_k, k = numColorsChosen
	// Add an edge from every precolored vertex to all nodes in K_k except its own color
	// Output: A converted precolored graph in DIMACS format with k = p
	for (i = 1; i < argc; i++) {
		if (strcmp("-r", argv[i]) == 0) {
			randomSeed = atoi(argv[++i]);
		}
		else if (strcmp("-v", argv[i]) == 0) {
			verbose++;
		}
		else if (strcmp("-p", argv[i]) == 0) {
			numPrecolorsChosen = atoi(argv[++i]);
		}
		else {
			ofstream resultsLog("resultsLog.log", ios::app);
			resultsLog << argv[i] << " sizeOfMaxIndepSet " << numNodesIndepSet.size() << " numUniqueColors " << candSol.size() << " numColorsChosen "<< numColorsChosen << endl;
			resultsLog.close();


			//Set up input file, read, and close (input must be in DIMACS format)
			ifstream in1;
			in1.open(argv[i]);

			ofstream newStrm;
			newStrm.open("newgraph.txt");

			newStrm << "c Adapting Precoloring Extensions problem to Graph Coloring problem\n"
				<< "c \n"
				<< "c Initial input graph has [" << numNodes << "] nodes and [" << numEdges << "] edges.\n"
				<< "c A total of [" << numNodesIndepSet.size() << "] nodes has been precolored with [" << numPrecolorsChosen << "] distinct number of colors.\n"
				<< "c Your input graph has been precolored and transformed.\n"
				<< "c It can now be run as an ordinary Graph Coloring Problem.\n"
				<< "c newgraph.txt is in DIMACS format.\n"
				<< "c *********************************************************************************\n"
				<< "a " << numPrecolorsChosen << "\n";
				for (int i = 0; i < numNodesIndepSet.size(); i++) {
					newStrm << "d " << numNodesIndepSet[i] +1 << "\n";
				}
			

			//newStrm << numNodes << '+' << candSol.size() << "\n";
			newStrm << "p edge " << numNodes + candSol.size() << ' ' << numEdges + ((candSol.size())*(candSol.size() - 1) / 2) << "\n";

			vector<int>::iterator pos1;
			pos1 = max_element(colNode.begin(), colNode.end());

			for (i = 0;i < numNodes;i++) {
				if (colNode[i] != -1) {
					for (int j = 0; j < *pos1 + 1; j++) {
						// add an edge between precolored node and all nodes in K_k except its own color 
						if (j != colNode[i]) {
							// i+1 because edges are defined with nodes that start from index 1
							// numNodes+1 because adding a complete graph to existing graph requires new nodes to start from index numNodes+1
							//FOR DEBUGGING: newStrm << "e " << i + 1 << ' ' << numNodes + 1 << '+' << j << ' ' << "\n";
							newStrm << "e " << i + 1 << ' ' << numNodes + 1 + j << "\n";
						}
					}
				}
			}

			// Specify edges for first K_k
			for (int g = numNodes + 1; g < numNodes + candSol.size() + 1; g++) {
				for (int f = numNodes + 1; f < numNodes + candSol.size() + 1; f++) {
					if (f != g) {
						newStrm << "e " << g << ' ' << f << "\n";
					}
				}
			}

			

			// Combine the K_k and new edges to original graph.txt file
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

			// if k is specified, algorithm outputs newnewgraph.txt for the specified k.
			if (prepareForGCP = 2) {
				preStepsGCP(numNodes, numEdges, numNodesIndepSet, numPrecolorsChosen, numColorsChosen, many);
				
			}

			// Produce a colorsolution.txt file, which shows the indices of vertices in their respective color classes
			ofstream colStrm;
			colStrm.open("colorsolution.txt");
			colStrm << candSol.size() << "\n";

			int k, count = 0, group;
			for (group = 0; group < candSol.size(); group++) {
				colStrm << "C-" << group << "\t= {";
				if (candSol[group].size() == 0) colStrm << "empty}\n";
				else {
					for (k = 0; k < candSol[group].size() - 1; k++) {
						colStrm << candSol[group][k]+1 << ", ";
					}
					colStrm << candSol[group][candSol[group].size() - 1]+1 << "}\n";
					count = count + candSol[group].size();
				}
			}
			colStrm << "Total Number of Nodes and Edges = " << '(' << numNodes << ", " << numEdges << ')' << endl;
			colStrm << "Number of Nodes in maximal independent set = " << numNodesIndepSet.size() << endl;
			colStrm << "Precolored vertices have " << candSol.size() << " unique number of color(s)." << endl;
			colStrm.close();

		}
	}
}
