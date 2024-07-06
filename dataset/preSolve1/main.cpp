#include "../../lib/config.cpp"


std::unordered_map<int, int> hpeId, nodeId;
int nowNodeId = 0;
int nowHyperId = 0;
std::unordered_map<int, std::vector<int>>maps;

int main(int argc, char *argv[]) {
    std::string dataset_name = argv[1];
    bugs(dataset_name);
    auto in = std::ifstream(std::string("./").append(dataset_name));
    auto out = std::ofstream(std::string("../preSolve2/").append(dataset_name));
    if (!in.is_open()) {
        bugs("Error: Cannot open file ", dataset_name);
        return 0;
    }
    if (!out.is_open()) {
        bugs("Error: Cannot open file ", dataset_name);
        return 0;
    }
    bugs("File ", std::string("./").append(dataset_name), " opened successfully");
    bugs("File ", std::string("../preSolve2/").append(dataset_name), " closed successfully");
    int node, hyperedge;
    while (in >> node >> hyperedge) {
        int thisNode, thisHyper;
        if (nodeId.find(node) == nodeId.end()) {
            nodeId[node] = ++nowNodeId;
            thisNode = nowNodeId;
        } else thisNode = nodeId[node];
        if (hpeId.find(hyperedge) == hpeId.end()) {
            hpeId[hyperedge] = ++nowHyperId;
            thisHyper = nowHyperId;
            maps[thisHyper] = std::vector<int>();
        } else thisHyper = hpeId[hyperedge];
        maps[thisHyper].push_back(thisNode);
    }
    for (auto & item : maps) {
        for (auto & node : item.second) {
            out << node << " ";
        }
        out << std::endl;
    }
    in.close();
    out.close();
    return 0;
}