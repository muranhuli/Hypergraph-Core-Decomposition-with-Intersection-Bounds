#include "../../lib/config.cpp"


std::unordered_map<int, int> hpeId, nodeId;
int nowNodeId = 0;
int nowHyperId = 0;
std::unordered_map<int, std::vector<int>>maps;

int main(int argc, char *argv[]) {
    std::string dataset_name = argv[1];
    bugs(dataset_name);
    auto in = std::ifstream(std::string("./").append(dataset_name));
    auto out = std::ofstream(std::string("../").append(dataset_name));
    if (!in.is_open()) {
        bugs("Error: Cannot open file ", dataset_name);
        return 0;
    }
    if (!out.is_open()) {
        bugs("Error: Cannot open file ", dataset_name);
        return 0;
    }
    bugs("File ", std::string("./").append(dataset_name), " opened successfully");
    bugs("File ", std::string("../").append(dataset_name), " opened successfully");
    auto line = std::string();
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        int node, size = 0;
        while (ss >> node) size++;
        if (size < 2) continue;
        out << line << "\n";
    }
    in.close();
    out.close();
    return 0;
}