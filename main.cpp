#include "lib/graph.cpp"

int main(int argc, char **argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0); 
    srand(time(0));
    // ./main dataset
    if (dataset_id.find(argv[1]) == dataset_id.end()) {
        std::cout << "dataset should be one of the following: ";
        for (auto it = dataset_id.begin(); it != dataset_id.end(); it++) {
            std::cout << it->first << " ";
        }
        std::cout << std::endl;
        return 0;
    }
    std::string dataset = argv[1];
    int datasetId = dataset_id[dataset];
    Graph *graph = new Graph(datasetId);
    graph->inputMap();
    // graph->initMap();
    graph->loadMap();
    // graph->analyIntersection();
    // graph->solveSCC();
    graph->solveStrongConntectedSCC();
    graph->calculateInformation();
    std::cout<<dataset<<" has finished!"<<std::endl;
    delete graph;
    return 0;
}