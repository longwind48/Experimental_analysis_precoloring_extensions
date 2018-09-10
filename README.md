# Experimental Analysis of Precoloring Extensions

By: Traci Lim

## Summary

The precoloring extension problem, or PrExt, is a computationally hard problem that belongs to a subfamily of graph coloring. The idea is to extend the coloring of a partially colored graph to a full one. The main challenge of real-life applications of PrExt is that ordinary graph coloring algorithms cannot be applied to precolored graphs because these algorithms do not take into account that colors of precolored vertices must stay unchanged. We developed a conversion algorithm that converts a precolored graph into a graph fit for any ordinary graph coloring algorithms, while retaining the fixed-color property of precolored vertices. The main highlight is a series of experimentation that focuses on applying local search heuristics to solve precoloring extension problems with the aim of discovering new results.

This repository contains the a copy of my [dissertation](traci_dissertation_final.pdf) completed for LSE's MSc Applicable Mathematics program. 



## Steps to run code

This section contains instructions on how to run the ***PrextToGCP*** and ***PartialColandTabuCol*** algorithm used in the experiments conducted. All algorithms are programmed in ```c++```. They are compiled in Windows using Microsoft Visual Studio Community 2017, Version 15.7.4. To run the programs attached, simply double-click on the ```.sln``` files if you have Microsoft Visual Studio installed.    

### Usage

- ***GenRandomGraphDensity***   

  Input: Graph Size, Density 

  Output: A random graph graph.txt in DIMACS format    

- ***PrextToGCP***    

  Example command: ```graph-1000-10.txt -r 1 -v -v -p 1 -c 23 -n 10``` 

  "```graph-1000-10.txt```" is the input graph in DIMACS format with 1000 vertices and 10% density. 

  "```-r 1```" sets the random seed to 1. 

  "```-v```" sets the verbosity. If present, output is sent to screen. If -v is repeated, more output is shown. 

  "```-p 1```" sets the number of unique colors in the precoloring to 1. 

  "```-c 23```" sets k = 23, where k is the k-coloring to be fixed. 

  "```-n 10```" outputs ten converted precolored graphs with 10 consecutive levels of k.    

  

  Output: ```precolorSolution.txt```: shows indices of vertices and its assigned color class (look at Code Listing 5.2 to understand how to interpret this file). 

  ```resultsLog.log```: shows history of commands newgraph.txt: A converted precolored graph with k = p. 

  ```newnewgraphX.txt```: A converted precolored graph with the specified k = X.    

- ***PartialColandTabuCol***    

  Example command: ```newnewgraph70.txt -t -tt -T 70 -v -v``` 

  "```newnewgraph70.txt```" is a converted precolored graph with k = 70 in DIMACS format 

  "```-t```" If present, TabuCol is used. Else PartialCol is used. 

  "```-tt```" If present, a dynamic tabu tenure is used, otherwise a reactive tenure is used. 

  "```-T 70```" sets target number of colours to be 70. Algorithm halts if this is reached. 

  "```-v```" sets the verbosity. If present, output is sent to screen. If -v is repeated, more output is shown. 

  Output: ```solution.txt```: shows indices of vertices and its assigned color class (which can be compared with ```precolorSolution.txt``` to confirm that vertices which were precolored gets the correct color, although the permutation might not be the same). 

  ```resultsLog.log```: shows history of commands, results, number of successes.    



### Workflow

A workflow of the experimental process is described in the following steps:    

1. Run *GenRandomGraphDensity* program to get the random graphs with desired densities.    

2. Copy the output file from step 1 to the main directory of *PrextToGCP*.

3. Run *PrextToGCP* to generate converted precolored graphs for some fixed k. 

   ```RandomGraphGenerated.txt -r 1 -v -v -p 1 -c 22 -n 10```.

4. Copy the output file(s) of *PrextToGCP* to the directory of *PartialColandTabuCol*.    

5. Run *PartialColandTabuCol* with the following commands to test all the heuristics. 

   ```newnewgraph22.txt -t -tt -T 22 -v -v ```

   ```newnewgraph22.txt -t -T 22 -v -v```

   ```newnewgraph22.txt -tt -T 22 -v -v```

   ```newnewgraph22.txt -T 22 -v -v    ```

6. Check ```resultsLog.log``` file to see which heuristic succeeded at least one time out of 5 seeds. If no k-coloring is found for all heuristics, go back to step 2 and set ```-c``` to 23.    

   

