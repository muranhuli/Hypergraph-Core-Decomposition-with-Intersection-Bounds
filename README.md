# Hypergraph-Core-Decomposition-with-Intersection-Bounds
This is the source code for Hypergraph-Core-Decomposition-with-Intersection-Bounds.

## Experimental Dataset
The dataset used in the experiments is available at http://konect.cc/networks/

We choose to process the bipartite graph into a hypergraph, with the left side as a hyperedge and the right side as a vertex, and remove the hyperedge of size 1.

## Running the code
1. Download the dataset from http://konect.cc/networks/ and process the dataset.
2. Run the following command
```
mkdir build && cd build
cmake ..
make
```
3. Executable file introduction
- `K_SCC_core` is the experimental procedure of this paper.
- `KH_Core` is the experimental procedure for the comparative experiment khcore.
- `Static` is the experimental procedure for analyzing subgraph properties.

## File Structure
- `lib/config.hpp` contains contains relevant configuration files.
- `lib/graph.hpp` contains the implementation of the algorithm of the paper.
- `lib/union.cpp` contains the implementation code of the union check.
- `main.cpp` contains the main function of the paper.
- `dataset` contains the code for processing the dataset.
- `khcore` contains the source code of the khcore.
