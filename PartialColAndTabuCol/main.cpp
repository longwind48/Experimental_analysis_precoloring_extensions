/******************************************************************************/
//  This code implements the PartialCol, and Tabucol graph coloring
//  heuristics described in "A Reactive Tabu Search Using Partial Solutions for the
//  Graph Coloring Problem" by Ivo Bloechliger and Nicolas Zuffery.
//	
//  The code was originally written by Ivo Bloechliger
//  http://rose.epfl.ch/~bloechli/coloring/
//  but has been modified for running with multiple values of k and for 
//  algorithm perfromance analysis
//	
//	See: Lewis, R. (2015) A Guide to Graph Colouring: Algorithms and Applications. Berlin, Springer. 
//       ISBN: 978-3-319-25728-0. http://www.springer.com/us/book/9783319257280
//	
//	for further details
/******************************************************************************/

#include "Graph.h"
#include "inputGraph.h"
#include "reactcol.h"
#include "tabu.h"
#include "manipulateArrays.h"
#include "initializeColoring.h"
#include <iomanip>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <string>

//This makes sure the compiler uses _strtoui64(x, y, z) with Microsoft Compilers, otherwise strtoull(x, y, z) is used
#ifdef _MSC_VER
  #define strtoull(x, y, z) _strtoui64(x, y, z)
#endif

using namespace std;

unsigned long long numConfChecks;

void usage() {
	cout<<"PartialCol and TabuCol Algorithm for Graph Colouring\n\n"
		<<"USAGE:\n"
		<<"<InputFile>     (Required. File must be in DIMACS format)\n"
		<<"-t              (If present, TabuCol is used. Else PartialCol is used.)\n"
		<<"-tt             (If present, a dynamic tabu tenure is used (i.e. tabuTenure = (int)(0.6*nc) + rand(0,9)). Otherwise a reactive tenure is used).\n"
		<<"-s <int>        (Stopping criteria expressed as number of constraint checks. Can be anything up to 9x10^18. DEFAULT = 100,000,000.)\n"
		<<"-r <int>        (Random seed. DEFAULT = 1)\n"
		<<"-T <int>        (Target number of colours. Algorithm halts if this is reached. DEFAULT = 1.)\n"
		<<"-v              (Verbosity. If present, output is sent to screen. If -v is repeated, more output is given.)\n"
		<<"-a <int>        (Choice of construction algorithm to determine initial value for k. DSsatur = 1, Greedy = 2. DEFAULT = 1.)\n"
		<<"****\n";
	exit(1);
}

int main(int argc, char ** argv)
{
	if (argc <= 1) {
		usage();
	}

	Graph g;
	bool miss=false;
	int k, frequency = 0, increment = 0, verbose = 0, randomSeed = 1, tenure = 0, algorithm = 1, cost, duration, constructiveAlg = 1, targetCols = 1, fail=0;
	unsigned long long maxChecks = INT_MAX;
	// INT_MAX
	// 800000000

	//Read in program parameters
	for (int i = 1; i < argc; i++) {
		if (strcmp("-s", argv[i]) == 0) {
			maxChecks = strtoull(argv[++i], NULL, 10);
		}
		else if (strcmp("-tt", argv[i]) == 0) {
			tenure++;
		}
		else if (strcmp("-t", argv[i]) == 0) {
			algorithm = 2;
		}
		else if (strcmp("-r", argv[i]) == 0) {
			randomSeed = atoi(argv[++i]);
		}
		else if (strcmp("-a", argv[i]) == 0) {
			constructiveAlg = atoi(argv[++i]);
		}
		else if (strcmp("-v", argv[i]) == 0) {
			verbose++;
		}
		else if (strcmp("-T", argv[i]) == 0) {
			targetCols = atoi(argv[++i]);
		}
		else {
			cout << "PartialCol/TabuCol Algorithm using <" << argv[i] << ">\n\n";
			inputDimacsGraph(g, argv[i]);
		}
	}

	if (targetCols < 2 || targetCols > g.n) targetCols = 2;

	//This variable keeps count of the number of times information about the instance is looked up 
	numConfChecks = 0;


	/////////////////////////// Uncomment this if you remove for-loop ///////////////////////////////////////
	//numConfChecks = 0;

	//srand(randomSeed + rand() % 100);

	////Now set up some output files
	//ofstream timeStream, confStream;
	//timeStream.open("teffort.txt"); confStream.open("ceffort.txt");
	//if (timeStream.fail() || confStream.fail()) { cout << "ERROR OPENING output FILE";exit(1); }

	////Do a check to see if we have the empty graph. If so, end immediately.
	//if (g.nbEdges <= 0) {
	//	confStream << "1\t0\n0\tX\t0\n";
	//	timeStream << "1\t0\n0\tX\t0\n";
	//	if (verbose >= 1) cout << "Graph has no edges. Optimal solution is obviously using one colour. Exiting." << endl;
	//	confStream.close();
	//	timeStream.close();
	//	exit(1);
	//}

	////Produce some output
	//if (verbose >= 1) cout << " COLS     CPU-TIME\tCHECKS" << endl;

	////Make the adjacency list structure 
	//int **neighbors = new int*[g.n];
	//makeAdjList(neighbors, g);

	////The solution is held in the following array
	//int *coloring = new int[g.n];
	//int *bestColouring = new int[g.n];

	////Now start the timer
	//clock_t clockStart = clock();

	////Generate the initial value for k using greedy or dsatur algorithm
	//k = generateInitialK(g, constructiveAlg, bestColouring);
	////..and write the results to the output file
	//duration = int(((double)(clock() - clockStart) / CLOCKS_PER_SEC) * 1000);
	//if (verbose >= 1) cout << setw(5) << k << setw(11) << duration << "ms\t" << numConfChecks << " (via constructive)" << endl;
	//confStream << k << "\t" << numConfChecks << "\n";
	//timeStream << k << "\t" << duration << "\n";

	////MAIN ALGORITHM
	//k--;
	//while (numConfChecks < maxChecks && k + 1 > targetCols) {

	//	//Initialise the solution array
	//	for (int i = 0; i < g.n; i++) coloring[i] = 0;

	//	//Do the algorithm for this value of k, either until a slution is found, or maxChecks is exceeded
	//	if (algorithm == 1) cost = reactcol(g, coloring, k, maxChecks, tenure, verbose, frequency, increment, neighbors);
	//	else cost = tabu(g, coloring, k, maxChecks, tenure, verbose, frequency, increment, neighbors);

	//	//Algorithm has finished at this k
	//	duration = int(((double)(clock() - clockStart) / CLOCKS_PER_SEC) * 1000);
	//	if (cost == 0) {
	//		if (verbose >= 1) cout << setw(5) << k << setw(11) << duration << "ms\t" << numConfChecks << endl;
	//		confStream << k << "\t" << numConfChecks << "\n";
	//		timeStream << k << "\t" << duration << "\n";
	//		//Copy the current solution as the best solution
	//		for (int i = 0; i < g.n; i++) bestColouring[i] = coloring[i] - 1;
	//		//Check if the target has been met
	//		if (k <= targetCols) {
	//			if (verbose >= 1) cout << "\nSolution with <=" << k << " colours has been found. Ending..." << endl;
	//			confStream << "1\t" << "X" << "\n";
	//			timeStream << "1\t" << "X" << "\n";
	//			break;
	//		}
	//	}
	//	else {
	//		if (verbose >= 1) cout << "\nRun limit exceeded. No solution using " << k << " colours was achieved (Checks = " << numConfChecks << ", " << duration << "ms)" << endl;
	//		confStream << k << "\tX\t" << numConfChecks << "\n";
	//		timeStream << k << "\tX\t" << duration << "\n";
	//		miss = true;
	//		fail++;

	//	}
	//	//Decrement k (if the run time hasn't been reached, we'll carry on with this new value)
	//	k--;
	//}

	// Run 5 times on 5 different seeds, comment out this for loop if only 1 run is needed
	for (int i = randomSeed; i < randomSeed + 5; i++) {
		numConfChecks = 0;

		srand(randomSeed+rand()%100);

		//Now set up some output files
		ofstream timeStream, confStream;
		timeStream.open("teffort.txt"); confStream.open("ceffort.txt");
		if (timeStream.fail() || confStream.fail()) { cout << "ERROR OPENING output FILE";exit(1); }

		//Do a check to see if we have the empty graph. If so, end immediately.
		if (g.nbEdges <= 0) {
			confStream << "1\t0\n0\tX\t0\n";
			timeStream << "1\t0\n0\tX\t0\n";
			if (verbose >= 1) cout << "Graph has no edges. Optimal solution is obviously using one colour. Exiting." << endl;
			confStream.close();
			timeStream.close();
			exit(1);
		}

		//Produce some output
		if (verbose >= 1) cout << " COLS     CPU-TIME\tCHECKS" << endl;

		//Make the adjacency list structure 
		int **neighbors = new int*[g.n];
		makeAdjList(neighbors, g);

		//The solution is held in the following array
		int *coloring = new int[g.n];
		int *bestColouring = new int[g.n];

		//Now start the timer
		clock_t clockStart = clock();

		//Generate the initial value for k using greedy or dsatur algorithm
		k = generateInitialK(g, constructiveAlg, bestColouring);
		//..and write the results to the output file
		duration = int(((double)(clock() - clockStart) / CLOCKS_PER_SEC) * 1000);
		if (verbose >= 1) cout << setw(5) << k << setw(11) << duration << "ms\t" << numConfChecks << " (via constructive)" << endl;
		confStream << k << "\t" << numConfChecks << "\n";
		timeStream << k << "\t" << duration << "\n";

		//MAIN ALGORITHM
		k--;
		while (numConfChecks < maxChecks && k + 1 > targetCols) {

			//Initialise the solution array
			for (int i = 0; i < g.n; i++) coloring[i] = 0;

			//Do the algorithm for this value of k, either until a slution is found, or maxChecks is exceeded
			if (algorithm == 1) cost = reactcol(g, coloring, k, maxChecks, tenure, verbose, frequency, increment, neighbors);
			else cost = tabu(g, coloring, k, maxChecks, tenure, verbose, frequency, increment, neighbors);

			//Algorithm has finished at this k
			duration = int(((double)(clock() - clockStart) / CLOCKS_PER_SEC) * 1000);
			if (cost == 0) {
				if (verbose >= 1) cout << setw(5) << k << setw(11) << duration << "ms\t" << numConfChecks << endl;
				confStream << k << "\t" << numConfChecks << "\n";
				timeStream << k << "\t" << duration << "\n";
				//Copy the current solution as the best solution
				for (int i = 0; i < g.n; i++) bestColouring[i] = coloring[i] - 1;
				//Check if the target has been met
				if (k <= targetCols) {
					if (verbose >= 1) cout << "\nSolution with <=" << k << " colours has been found. Ending..." << endl;
					confStream << "1\t" << "X" << "\n";
					timeStream << "1\t" << "X" << "\n";
					break;
				}
			}
			else {
				if (verbose >= 1) cout << "\nRun limit exceeded. No solution using " << k << " colours was achieved (Checks = " << numConfChecks << ", " << duration << "ms)" << endl;
				confStream << k << "\tX\t" << numConfChecks << "\n";
				timeStream << k << "\tX\t" << duration << "\n";
				miss = true;
				fail++;

			}
			//Decrement k (if the run time hasn't been reached, we'll carry on with this new value)
			k--;
			//output the solution to a text file
			ofstream solStrm;
			solStrm.open("solution.txt");
			solStrm << g.n << "\n";
			for (int i = 0;i < g.n;i++) solStrm << i + 1 << ' ' << bestColouring[i] << "\n";
			solStrm.close();
		}

	}

	//output the solution to a text file
	//ofstream solStrm;
	//solStrm.open("solution.txt");
	//solStrm << g.n << "\n";
	//for (int i = 0;i < g.n;i++) solStrm << i + 1 << ' ' << bestColouring[i] << "\n";
	//solStrm.close();


	// Maintains a log file that shows your history
	ofstream resultsLog("resultsLog.log", ios::app);
	if (miss==false || fail<5) {
		if ((tenure == 1) && (algorithm == 1))
			resultsLog << "partialcol " << "targetK " << targetCols << " dynamic " << k << " HIT " << 5-fail << endl;
		if ((tenure == 1) && (algorithm == 2))
			resultsLog << "tabucol " << "targetK " << targetCols << " dynamic " << k << " HIT " << 5 - fail << endl;
		if ((tenure == 0) && (algorithm == 1))
			resultsLog << "partialcol " << "targetK " << targetCols << " reactive " << k << " HIT " << 5 - fail <<endl;
		if ((tenure == 0) && (algorithm == 2))
			resultsLog << "tabucol " << "targetK " << targetCols << " reactive " << k << " HIT " << 5 - fail << endl;
	}
	else {
		if ((tenure == 1) && (algorithm == 1))
			resultsLog << "partialcol " << "targetK " << targetCols << " dynamic " << k << " MiSS " << 5 - fail << endl;
		if ((tenure == 1) && (algorithm == 2))
			resultsLog << "tabucol " << "targetK " << targetCols << " dynamic " << k << " MiSS " << 5 - fail << endl;
		if ((tenure == 0) && (algorithm == 1))
			resultsLog << "partialcol " << "targetK " << targetCols << " reactive " << k << " MiSS " << 5 - fail << endl;
		if ((tenure == 0) && (algorithm == 2))
			resultsLog << "tabucol " << "targetK " << targetCols << " reactive " << k << " MISS " << 5 - fail << endl;
	}
	resultsLog.close();
	

	/////////////////////////// Uncomment this if you remove for-loop ///////////////////////////////////////
	//Delete the arrays and end	
	//confStream.close();
	//timeStream.close();
	//delete[] coloring;
	//delete[] bestColouring;
	//for (int i = 0;i < g.n;i++) delete[] neighbors[i];
	//delete[] neighbors;

	}


