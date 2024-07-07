#ifndef GRAPH_H
#define GRAPH_H
#include "config.cpp"
#include "union_find.cpp"

typedef long long ll;
/**
 * @def Node
 * id: node id
 * degree: number of hyperedges that contain this node
 * core: max k that this node is in k-core
*/
struct Node {
    int id;
    int degree;
    int _degree;
    int core;
    int scc;
    Node(int id){
        this->id = id;
        this->degree = 0;
        _degree = 0;
        core = -1;
        scc = 0;
    }
    inline bool operator<(const Node &node) const {
        return scc < node.scc;
    }
};
/**
 * @def HyperEdge
 * id: hyperedge id
 * nodes: nodes that contain this hyperedge
 * degree: the number of nodes that contain this hyperedge
 * core: the minimum node's core in this hyperedge
*/
struct HyperEdge{
    int id;
    std::unordered_set<Node *> nodes;
    int degree;
    int core;
    bool selected;
    int maxS, k, s;
    HyperEdge(int id, int degree){
        nodes.clear();
        this->id = id;
        this->degree = degree;
        core = -1;
        selected = false;
        maxS = 0;
        k = -1;
        s = -1;
    }
};
/**
 * @def Graph
 * nodes: all nodes in the graph
 * hyperedges: all hyperedges in the graph
 * num_nodes: number of nodes in the graph
 * num_hyperedges: number of hyperedges in the graph
 * @brief inputMap() reads the datasetName file and stores the graph in the Graph object
*/
struct Graph {
    std::unordered_map<int, Node *> nodes;
    std::vector<HyperEdge *> hyperedges;
    int num_nodes;
    int num_hyperedges;
    int datasetID;
    std::string datasetName;
    void inputMap();
    void solveCore(std::unordered_map<Node *, int> &unSolvedNodes, std::vector<HyperEdge *> &hyperedges);
    void queryKP(int k, int p, const std::string& funcId);
    std::unordered_map<Node *, int> initCore(std::vector<HyperEdge *> &hyperedges);
    void queryBase(int k, int p, const std::string& funcId);
    Graph(int datasetId);
    ~Graph();
    std::unordered_map<HyperEdge*, std::vector<Node*>> EtoVMap;
    std::unordered_map<Node*, std::vector<HyperEdge*>> VtoEMap;
    std::unordered_map<HyperEdge*, std::unordered_map<HyperEdge*, int>> EtoEMap;
    std::unordered_map<int, std::vector<HyperEdge*>> StoEMap;
    std::unordered_map<int, std::vector<HyperEdge*>> CoreE;
    std::unordered_map<HyperEdge*, int> ECore;
    std::unordered_map<HyperEdge*, std::vector<std::pair<HyperEdge*, int>>> EtoSMap;
    void storeMap();
    void loadMap();
    void initMap();
    void solveSCC();
    void analyIntersection();
    void calculateBaseInformation(std::ofstream &fout, int k, int s);
    void calculateInformation();
    void solveStrongConntectedSCC();
};

Graph::Graph(int datasetId) {
    nodes.clear();
    hyperedges.clear();
    num_nodes = 0;
    num_hyperedges = 0;
    this->datasetID = datasetId;
    datasetName = datasetNames[datasetID];
}
Graph::~Graph() {
    for (auto node : nodes) {
        delete node.second;
    }
    for (auto hyperedge : hyperedges) {
        delete hyperedge;
    }
}

void Graph::inputMap() {
    std::cout << "===========begin inputMap============" << std::endl;
    std::ifstream file;
    file.open("/media/disk7T/liuyu/SCC/dataset/" + datasetName);
    if (!file) {
        std::cerr << "Unable to open file" << std::endl;
        throw;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        std::vector<int> nodeIDs;
        while (ss >> token) {
            nodeIDs.push_back(std::stoi(token));
        }
        HyperEdge *hyperedge = new HyperEdge(num_hyperedges++, nodeIDs.size());
        hyperedges.push_back(hyperedge);
        for (auto nodeID : nodeIDs) {
            if (nodes.find(nodeID) == nodes.end()) {
                Node *node = new Node(nodeID);
                nodes[nodeID] = node;
                num_nodes = std::max(num_nodes, nodeID);
            }
            hyperedge->nodes.insert(nodes[nodeID]);
            nodes[nodeID]->degree++;
        }
    }
    file.close();
    std::cout << "datasetName: " << datasetName << std::endl;
    std::cout << "num_nodes: " << num_nodes << std::endl;
    std::cout << "num_hyperedges: " << num_hyperedges << std::endl;
    std::cout << "===========end inputMap============" << std::endl;
}

void Graph::solveCore(std::unordered_map<Node *, int> &unSolvedNodes, std::vector<HyperEdge *> &hyperedges) {
    int k = 0;
    // std::unordered_map<int, std::vector<Node *>> degreeK;
    // int maxDegree = 0;
    // for (auto item : unSolvedNodes) {
    //     auto node = item.first;
    //     auto degree = node->_degree[p];
    //     maxDegree = std::max(maxDegree, degree);
    //     if (degreeK.find(degree) == degreeK.end()) {
    //         degreeK[degree] = std::vector<Node *>();
    //     }
    //     degreeK[degree].push_back(node);
    // }
    // 要设定所有节点的 core
    while (!unSolvedNodes.empty()) {
        std::queue<Node *> S;
        // 找出所有 _degree <= k 的节点
        for (auto item : unSolvedNodes) {
            auto node = item.first;
            if (node->_degree <= k && unSolvedNodes[node] != 1) {
                S.push(node);
                unSolvedNodes[node] = 1;
            }
        }
        // for (auto item : degreeK[k]) {
        //     if (unSolvedNodes[item] != 1) {
        //         S.push(item);
        //         unSolvedNodes[item] = 1;
        //     }
        // }
        // 从 S 中取出节点，设定 core
        while (!S.empty()) {
            auto top = S.front();
            S.pop();
            top->core = k;
            // 从 unSolvedNodes 中删除
            if (unSolvedNodes.erase(top) != 1) {
                std::cout << "unSolvedNodes.erase(top) != 1" << std::endl;
                throw;
            }
            // 更新所有与 top 相连的节点的 _degree
            for (auto hyperedge : hyperedges) {
                // 如果 hyperedge 已经设定了 core(已经被删除)，则跳过
                if (hyperedge->core != -1) continue;
                // 如果 hyperedge 中不包含 node，则跳过
                if (hyperedge->nodes.find(top) == hyperedge->nodes.end()) continue;
                // 如果 hyperedge 中包含 nodem, hyperedge 的 core 的定义是最小的 core
                // 那么当 hyperedge 中的节点有一个被设定了 core，那么 hyperedge 的 core 也就确定了,因为它一定是最小的
                hyperedge->core = k;
                // 那么这条 hyperedge 中的其他节点的 _degree 就可以减一了
                // 更新 hyperedge 中所有节点的 _degree(因为这条边已经被删除了)
                for (auto node : hyperedge->nodes) {
                    if (node->core != -1) continue;
                    node->_degree--;
                    if (node->_degree <= k && unSolvedNodes[node] != 1) {
                        S.push(node);
                        unSolvedNodes[node] = 1;
                    }
                }
            }
        }
        k++;
    }
}

std::unordered_map<Node *, int> Graph::initCore(std::vector<HyperEdge *> &hyperedges) {
    std::unordered_map<Node *, int> unSolvedNodes;
    for (auto hyperedge : hyperedges) {
        hyperedge->core = -1;
        for (auto node : hyperedge->nodes) {
            if (unSolvedNodes.find(node) == unSolvedNodes.end()) {
                unSolvedNodes.insert(std::make_pair(node, 0));
                node->_degree = 0;
                node->core = -1;
            }
            node->_degree++;
        }
    }
    return unSolvedNodes;
}

void Graph::queryKP(int k, int p, const std::string& funcId) {
    auto start = std::chrono::steady_clock::now();
    std::vector<HyperEdge *> hyperedges(this->hyperedges);
    // 初始化 core
    auto unSolvedNodes = initCore(hyperedges);
    std::cout << "begin kcore" << std::endl;
    // 在 满足func(p) 的 hyperedge 中，计算 core
    auto time1 = std::chrono::steady_clock::now();
    solveCore(unSolvedNodes, hyperedges);
    auto time2 = std::chrono::steady_clock::now();
    std::cout << "kcore time = " << (time2-time1) / 1ms << "ms" << std::endl;
    // 筛选出 满足 core >= k 的 hyperedge
    std::vector<HyperEdge *> ansEdge;
    for (auto edge : hyperedges) {
        if (edge->core >= k) {
            ansEdge.push_back(edge);
        }
    }
    dsu *d = new dsu(num_nodes);
    for (auto edge : ansEdge) {
        for (auto node : edge->nodes) {
            d->unite((*edge->nodes.begin())->id, node->id);
        }
    }
    std::unordered_map<int, std::vector<HyperEdge *>> ans;
    for (auto edge : ansEdge) {
        int root = d->find((*edge->nodes.begin())->id);
        if (ans.find(root) == ans.end()) ans[root] = std::vector<HyperEdge *>();
        ans[root].push_back(edge);
    }
    auto end = std::chrono::steady_clock::now();
    printf("queryKP: k=%d p=%d func=%s\n", k, p, funcId.c_str());
    std::cout << "time = " << (end-start) / 1ns << "ns" << std::endl;
    std::cout << "hyperedges.size(): " << ansEdge.size() << std::endl;
    std::cout << "connected components: " << ans.size() << std::endl;
    for (auto item : ans) {
        std::cout << "connected components root= " << item.first << " size: " << item.second.size() << std::endl;
    }
}

void Graph::initMap() {
    Timer::measure(datasetName + " solve EtoVMap", [&](){
        for (auto edge : hyperedges) {
            for (auto node : edge->nodes) {
                if (EtoVMap.find(edge) == EtoVMap.end()) {
                    EtoVMap[edge] = std::vector<Node*>();
                }
                EtoVMap[edge].push_back(node);
            }
        }
    });
    Timer::measure(datasetName + " solve VtoEMap", [&](){
        for (auto edge : hyperedges) {
            for (auto node : edge->nodes) {
                if (VtoEMap.find(node) == VtoEMap.end()) {
                    VtoEMap[node] = std::vector<HyperEdge*>();
                }
                VtoEMap[node].push_back(edge);
            }
        }
    });
    Timer::measure(datasetName + " solve EtoEMap", [&](){
        /**
         * EtoEMap  边的邻接边的map，且知道与这条邻接边的intersection size （其实是就是构造了一个weighted line graph的邻接链表）最重要 
         * key为边; value是边的submap；submap的key为边，value为instersection size
        */ 
       for (auto & edge: hyperedges) {
            EtoEMap[edge] = std::unordered_map<HyperEdge*, int>();
            for (auto & node: edge->nodes) {
                for (auto & adjEdge: VtoEMap[node]) {
                    if (adjEdge == edge) continue;
                    if (EtoEMap[edge].find(adjEdge) == EtoEMap[edge].end()) {
                        EtoEMap[edge][adjEdge] = 0;
                    }
                    EtoEMap[edge][adjEdge]++;
                }
            }
       }
    });
    Timer::measure(datasetName + " solve StoEMap", [&](){
        // min intersection size to edge list map
        for (auto & edge: hyperedges) {
            int min = INT_MAX;
            for (auto & other : EtoEMap[edge]) {
                min = std::min(min, other.second);
            }
            if (StoEMap.find(min) == StoEMap.end()) {
                StoEMap[min] = std::vector<HyperEdge*>();
            }
            StoEMap[min].push_back(edge);
        }
    });
    Timer::measure(datasetName + " solve CoreE  ECore", [&](){
        auto ret = initCore(this->hyperedges);
        solveCore(ret, this->hyperedges);
        for (auto edge : hyperedges) {
            if (CoreE.find(edge->core) == CoreE.end()) {
                CoreE[edge->core] = std::vector<HyperEdge*>();
            }
            CoreE[edge->core].push_back(edge);
            ECore[edge] = edge->core;
        }
    });
    Timer::measure(datasetName + " solve EtoSMap", [&](){
        // EtoSMap】边与其邻接边的instersection order （由上一个EtoEMap可计算）
        // key为边; value为一个ordered intersection size list，不包含重复元素，值从大到小排列
        for (auto & edge: EtoEMap) {
            EtoSMap[edge.first] = std::vector<std::pair<HyperEdge*, int>>();
            for (auto & adjEdge: edge.second) {
                EtoSMap[edge.first].push_back(adjEdge);
            }
            std::sort(EtoSMap[edge.first].begin(), EtoSMap[edge.first].end(), [](std::pair<HyperEdge*, int> &a, std::pair<HyperEdge*, int> &b){
                return a.second > b.second;
            });
        }
    });
    storeMap();
}
void Graph::storeMap() {
    std::ofstream file("/media/disk7T/liuyu/SCC/dataset/index/" + datasetName + ".pre");
    // EtoVMap
    file << EtoVMap.size() << "\n";
    for (auto & edge: EtoVMap) {
        file << edge.first->id << " " << edge.second.size() << " ";
        for (auto & node: edge.second) {
            file << node->id << " ";
        }
        file << "\n";
    }
    // VtoEMap
    file << VtoEMap.size() << "\n";
    for (auto & node: VtoEMap) {
        file << node.first->id << " " << node.second.size() << " ";
        for (auto & edge: node.second) {
            file << edge->id << " ";
        }
        file << "\n";
    }
    // EtoEMap
    file << EtoEMap.size() << "\n";
    for (auto & edge: EtoEMap) {
        file << edge.first->id << " " << edge.second.size() << " ";
        for (auto & adjEdge: edge.second) {
            file << adjEdge.first->id << " " << adjEdge.second << " ";
        }
        file << "\n";
    }
    // StoEMap
    file << StoEMap.size() << "\n";
    for (auto & item: StoEMap) {
        file << item.first << " " << item.second.size() << " ";
        for (auto & edge: item.second) {
            file << edge->id << " ";
        }
        file << "\n";
    }
    // CoreE
    file << CoreE.size() << "\n";
    for (auto & item: CoreE) {
        file << item.first << " " << item.second.size() << " ";
        for (auto & edge: item.second) {
            file << edge->id << " ";
        }
        file << "\n";
    }
    // ECore
    file << ECore.size() << "\n";
    for (auto & item: ECore) {
        file << item.first->id << " " << item.second << "\n";
    }
    // EtoSMap
    file << EtoSMap.size() << "\n";
    for (auto & edge: EtoSMap) {
        file << edge.first->id << " " << edge.second.size() << " ";
        for (auto & adjEdge: edge.second) {
            file << adjEdge.first->id << " " << adjEdge.second << " ";
        }
        file << "\n";
    }
    file.close();
}
void Graph::loadMap() {
    std::ifstream file("/media/disk7T/liuyu/SCC/dataset/index/" + datasetName + ".pre");
    // load EtoVMap
    int size;
    file >> size;
    for (int i = 0; i < size; i++) {
        int id, n;
        file >> id >> n;
        EtoVMap[hyperedges[id]] = std::vector<Node*>(n);
        for (int j = 0; j < n; j++) {
            int node;
            file >> node;
            EtoVMap[hyperedges[id]][j] = nodes[node];
        }
    }
    // load VtoEMap
    file >> size;
    for (int i = 0; i < size; i++) {
        int id, n;
        file >> id >> n;
        VtoEMap[nodes[id]] = std::vector<HyperEdge*>(n);
        for (int j = 0; j < n; j++) {
            int edge;
            file >> edge;
            VtoEMap[nodes[id]][j] = hyperedges[edge];
        }
    }
    // load EtoEMap
    file >> size;
    for (int i = 0; i < size; i++) {
        int id, n;
        file >> id >> n;
        EtoEMap[hyperedges[id]] = std::unordered_map<HyperEdge*, int>();
        for (int j = 0; j < n; j++) {
            int adjEdge, intersection;
            file >> adjEdge >> intersection;
            EtoEMap[hyperedges[id]][hyperedges[adjEdge]] = intersection;
        }
    }
    // load StoEMap
    file >> size;
    for (int i = 0; i < size; i++) {
        int min, n;
        file >> min >> n;
        StoEMap[min] = std::vector<HyperEdge*>(n);
        for (int j = 0; j < n; j++) {
            int edge;
            file >> edge;
            StoEMap[min][j] = hyperedges[edge];
        }
    }
    // load CoreE
    file >> size;
    for (int i = 0; i < size; i++) {
        int core, n;
        file >> core >> n;
        CoreE[core] = std::vector<HyperEdge*>(n);
        for (int j = 0; j < n; j++) {
            int edge;
            file >> edge;
            CoreE[core][j] = hyperedges[edge];
        }
    }
    // load ECore
    file >> size;
    for (int i = 0; i < size; i++) {
        int id, core;
        file >> id >> core;
        ECore[hyperedges[id]] = core;
    }
    // load EtoSMap
    file >> size;
    for (int i = 0; i < size; i++) {
        int id, n;
        file >> id >> n;
        EtoSMap[hyperedges[id]] = std::vector<std::pair<HyperEdge*, int>>(n);
        for (int j = 0; j < n; j++) {
            int adjEdge, intersection;
            file >> adjEdge >> intersection;
            EtoSMap[hyperedges[id]][j] = std::make_pair(hyperedges[adjEdge], intersection);
        }
    }
    file.close();
}

void Graph::solveSCC() {
    // outputFile
    std::ofstream file("./result/kscore/basic_information_1_"+ datasetName + ".csv");
    file << "datasetName" << "," << "k" << "," << "s" << "," << "time " << "us," << "kscoreNum" << std::endl;
    // bcj
    dsu *d = new dsu(num_nodes + 100);
    std::set<Node *> dsuNode;
    std::set<HyperEdge *> dsuEdge;
    // 现在取出来的所有边
    std::vector<HyperEdge *> allHyperEdge;
    // 现在取出来的所有节点
    std::vector<Node *> allNode;
    // 所有可选的 k, 从大到小排序
    std::vector<int> kSelect;
    {
        for (auto & tmp : CoreE) kSelect.push_back(tmp.first);
        std::sort(kSelect.begin(), kSelect.end(), std::greater<int>());
    }
    // 从大到小遍历所有的 k
    for (auto & k : kSelect) {
        // 取出所有的新加入的边
        const std::vector<HyperEdge *> &newHyperEdge_k = CoreE[k];
        for (auto & edge : newHyperEdge_k) edge->selected = true;
        std::unordered_map<int, std::vector<HyperEdge *>> newStoEMap;
        // 计算这些边的最大的 S 值
        for (auto & edge : newHyperEdge_k) {
            for (auto & adjEdge : EtoSMap[edge]) {
                if (adjEdge.first->selected == false) continue;
                edge->maxS = adjEdge.second;
                if (newStoEMap.find(edge->maxS) == newStoEMap.end()) 
                    newStoEMap[edge->maxS] = std::vector<HyperEdge *>();
                newStoEMap[edge->maxS].push_back(edge);
                break;
            }
            if (edge->maxS == 0) {
                if (newStoEMap.find(0) == newStoEMap.end()) newStoEMap[0] = std::vector<HyperEdge *>();
                newStoEMap[0].push_back(edge);
            }
        }
        // 得到当前的所有的 S 取值
        std::vector<int> sSelect;
        for (auto & tmp : newStoEMap) sSelect.push_back(tmp.first);
        std::sort(sSelect.begin(), sSelect.end(), std::greater<int>());
        std::map<int, int> node_scc;
        auto update = [&](int from, int to) {
            if (node_scc.find(from) == node_scc.end()) { // from = 0
                node_scc.find(to) == node_scc.end() ? node_scc[to] = 1 : node_scc[to]++;
            } else {
                node_scc[from]--;
                if (node_scc[from] == 0) node_scc.erase(from);
                node_scc.find(to) == node_scc.end() ? node_scc[to] = 1 : node_scc[to]++;
            }
        };
        auto nowTime = std::chrono::steady_clock::now();
        // 从大到小遍历所有的 S
        for (auto & s : sSelect) {
            const auto & newHyperEdge_s = newStoEMap[s];
            for (auto & edge : newHyperEdge_s) {
                edge->k = k;
                for (auto & node : EtoVMap[edge]) {
                    update(node->scc, node->scc + 1);
                    node->scc++;
                    d->unite((*edge->nodes.begin())->id, node->id);
                    dsuNode.insert(node);
                }
                dsuEdge.insert(edge);
            }
            // 如果最小的节点的 node 的 scc 小于 k，那就继续减少 s
            if (node_scc.begin()->first < k) continue;
            else {
                // 如果所有的节点的 scc 都大于等于 k，那么就可以确定这个 s 是最小的
                // 所有标记了 k 但是没标记 s 的边，标记 s(标记 k 的表示在 s 更大的时候已经统计过了)
                for (auto & edge : newHyperEdge_k) {
                    if (edge->k == k && edge->s == -1) edge->s = s;
                }
                // 输出
                // 计算连通块数量
                std::set<int> sccSet;
                for (auto & node : dsuNode) sccSet.insert(d->find(node->id));
                int kscoreNum = sccSet.size();
                double Overlapping = 0.0;
                for (auto & edge: dsuEdge) Overlapping += edge->nodes.size();
                Overlapping /= dsuNode.size(); 
                int s_density_sum = 0;
                for (auto & edge : dsuEdge) {
                    for (auto & adjEdge : EtoSMap[edge]) {
                        if (adjEdge.first->selected == false) continue;
                        if (adjEdge.second < s) break;
                        s_density_sum++;
                    }
                }
                // double s_density = double(s_density_sum) / 2 / (dsuEdge.size() * (dsuNode.size() - 1) / 2.0);
                file << datasetName << "," << k << "," << s << "," << (std::chrono::steady_clock::now() - nowTime) / 1us << "us," << kscoreNum << std::endl;
                nowTime = std::chrono::steady_clock::now();
            }
        }
    }
    file.close();
    // 输出每个边的KS-Core信息
    file.open("/media/disk7T/liuyu/SCC/dataset/index/"+ datasetName + ".core");
    for (auto & edge : hyperedges) {
        file << edge->id << " " << edge->k << " " << edge->s << std::endl;
    }
    file.close();
}

void Graph::analyIntersection()
{
    std::map<int,int> intersection;
    for (size_t i=0;i<EtoEMap.size();i++) {
        for (auto &pair: EtoEMap[hyperedges[i]]) {
            if (hyperedges[i]->id > pair.first->id)
                intersection[pair.second]++;
        }
    }
    int sum=0;
    for (auto &pair: intersection) {
        sum+=pair.second;
    }
    int tmp=0;

    std::ofstream fout("result/"+datasetName+".txt");
    fout << "intersection size\t#intersections\tpercentage\t#cumulative percentage"<<std::endl;
    for (auto &pair: intersection) {
        tmp+=pair.second;
        fout << pair.first << "\t" << pair.second <<"\t"<< double(pair.second*1.0)/double(sum)<<"\t"<<double(tmp*1.0)/double(sum)<<std::endl;
    }
    fout.close();
}

void Graph::calculateBaseInformation(std::ofstream &fout, int k, int s)
{
    // 计算联通子图数
    auto nowTime = std::chrono::steady_clock::now();
    dsu *d = new dsu(num_nodes + 100);
    std::set<Node *> dsuNode;
    for (auto & edge: hyperedges)
    {
        if (edge->selected)
        {
            for (auto & node: EtoVMap[edge])
            {
                d->unite((*edge->nodes.begin())->id, node->id);
                dsuNode.insert(node);
            }
        }
    }
    std::set<int> sccSet;
    for (auto & node : dsuNode) sccSet.insert(d->find(node->id));
    int kscoreNum = int(sccSet.size());
    auto time = (std::chrono::steady_clock::now()-nowTime)/1us;

    int edge_num=0;
    int edge_length=0;
    int node_num=0;
    std::unordered_set<int> node_set;
    for(auto & edge: hyperedges)
    {
        if (edge->selected)
        {
            edge_num++;
            for (auto & node : EtoVMap[edge])
            {
                node_set.insert(node->id);
                edge_length++;
            }
        }
    }
    node_num = int(node_set.size());
    double overlapping = double(edge_length)/double(node_num);
    double nodeEdgeRatio = double(node_num)/double(edge_num);

    int insection_num=0;
    for (size_t i=0;i<EtoEMap.size();i++)
    {
        if (!hyperedges[i]->selected)
            continue;
        for (auto &pair: EtoEMap[hyperedges[i]])
        {
            if (!pair.first->selected)
                continue;
            if (hyperedges[i]->id > pair.first->id)
                continue;
            if (pair.second >= s)
                insection_num++;

        }
    }
    double intersection_density = double(insection_num)/double(ll(edge_num)*(ll(edge_num)-1)/2);
    fout<<datasetName<<","<<k<<","<<s<<","<<time<<","<<kscoreNum<<","<<overlapping<<","<<nodeEdgeRatio<<","<<intersection_density<<std::endl;
}

void Graph::calculateInformation()
{
    // 读取kscore
    std::ifstream fin("/media/disk7T/liuyu/SCC/dataset/index/"+ datasetName + ".strongConnected.core");
    std::ofstream file("./result/strongConnectKScore/basic_information_2_"+ datasetName + ".csv");
    file<<"datasetName"<<","<<"k"<<","<<"s"<<",connectedTime,kscoreNum,"<<"overlapping"<<","<<"nodeEdgeRatio"<<","<<"intersection_density"<<std::endl;
    int a,b,c;
    std::map<std::pair<int, int>, std::unordered_set<int>> core;
    while (fin>>a>>b>>c)
    {
        hyperedges[a]->k = b;
        hyperedges[a]->s = c;
        core[std::make_pair(b,c)].insert(a);
    }
    fin.close();
    for (auto & edge: hyperedges)
    {
        edge->selected=true;
    }

    for(auto & pair: core)
    {
        calculateBaseInformation(file, pair.first.first, pair.first.second);
        for(auto edge: pair.second)
        {
            hyperedges[edge]->selected=false;
        }
    }
    file.close();
}

void Graph::solveStrongConntectedSCC()
{
    std::ofstream file("./result/strongConnectKScore/basic_information_1_"+ datasetName + ".csv");
    file << "datasetName" << "," << "k" << "," << "s" << "," << "time " << "us"<< std::endl;

    std::set<Node *> dsuNode;
    std::set<HyperEdge *> dsuEdge;
    // 现在取出来的所有边
    std::vector<HyperEdge *> allHyperEdge;
    // 现在取出来的所有节点
    std::vector<Node *> allNode;
    // 所有可选的 k, 从大到小排序
    std::vector<int> kSelect;
    {
        for (auto &tmp: CoreE) kSelect.push_back(tmp.first);
        std::sort(kSelect.begin(), kSelect.end());
    }

    for (auto &edge: hyperedges)
        edge->selected = true;

    // 定义超边的信息结构
    struct HyperedgeInfo
    {
        std::multiset<int, std::greater<>> insectionOrder; // 多重集合(multiset)，按照整数值从大到小排序，存储该超边与其他超边的交集大小。
        int selfSize;  // 超边自身的大小
        std::multiset<int, std::greater<>> incidentSizeOrder; // 存储与该超边相邻的其他超边的大小
        int ID;
    };
    // 定义超边的删除函数
    struct CompareHyperedgeInfo
    {
        bool operator()(const HyperedgeInfo &a, const HyperedgeInfo &b) const
        {
            if (a.insectionOrder.empty())
                return true;
            else if (b.insectionOrder.empty())
                return false;
            int aLast = *a.insectionOrder.rbegin();
            int bLast = *b.insectionOrder.rbegin();
            if (aLast != bLast)
            {
                return aLast < bLast;
            }
            if (a.insectionOrder != b.insectionOrder)
            {
                return a.insectionOrder < b.insectionOrder;
            }
            if (a.selfSize != b.selfSize)
            {
                return a.selfSize < b.selfSize;
            }
            if (a.incidentSizeOrder != b.incidentSizeOrder)
            {
                return a.incidentSizeOrder < b.incidentSizeOrder;
            }
            return a.ID < b.ID;
        }
    };

    // 从小到大遍历所有的 k
    for (auto &k: kSelect)
    {
        std::unordered_map<int, int> deg;
        for(auto & edge: hyperedges)
        {
            if (edge->selected)
            {
                for (auto & node: edge->nodes)
                {
                    if (deg.find(node->id) == deg.end())
                    {
                        deg[node->id] = 0;
                    }
                    deg[node->id]++;
                }
            }
        }
        // 存储超边的信息合计
        std::set<HyperedgeInfo, CompareHyperedgeInfo> hyperedgeInfoSet;
        std::unordered_map<int, HyperedgeInfo> hyperedgeInfoMap;
        // 取出所有的新加入的边
        const std::vector<HyperEdge *> &newHyperEdge_k = CoreE[k];
        for (auto & edge : newHyperEdge_k)
        {
            edge->k = k;
            HyperedgeInfo hyperedgeInfo;
            hyperedgeInfo.selfSize = int(edge->nodes.size());
            hyperedgeInfo.ID = edge->id;
            for (auto & neiE : EtoEMap[edge])
            {
                if (neiE.first->selected)
                {
                    hyperedgeInfo.incidentSizeOrder.insert(int(neiE.first->nodes.size()));
                    hyperedgeInfo.insectionOrder.insert(neiE.second);
                }
            }
            hyperedgeInfoSet.insert(hyperedgeInfo);
            hyperedgeInfoMap[hyperedgeInfo.ID]=hyperedgeInfo;
        }
        int s = 0;
        int tmp_s = -1;
        auto nowTime = std::chrono::steady_clock::now();
        while(!hyperedgeInfoSet.empty())
        {
            HyperedgeInfo hyperedgeInfo = *hyperedgeInfoSet.begin();
            if (!hyperedgeInfo.insectionOrder.empty())
            {
                s = std::max(s, *(hyperedgeInfo.insectionOrder.rbegin()));
            }
            // 判断剩余的子图是不是k-core
            // 更新剩余边的ks值
            int minDeg = INT_MAX;
            for (auto& pair: deg)
            {
                minDeg = std::min(minDeg, pair.second);
            }
            if (minDeg > k)
            {
                break;
            }
            else if (minDeg == k)
            {
                if (tmp_s<s)
                {
                    file << datasetName << "," << k << "," << s << "," << (std::chrono::steady_clock::now() - nowTime) / 1us << "us"<<std::endl;
                }
                for (auto &edge: hyperedges)
                    if (edge->selected and edge->k == k)
                        edge->s = s;
                tmp_s = s;
            }
            // 删除该边，更新相应的删除信息
            hyperedgeInfoSet.erase(hyperedgeInfoSet.begin());
            hyperedgeInfoMap.erase(hyperedgeInfo.ID);
            hyperedges[hyperedgeInfo.ID]->selected = false;
            for (auto & node: hyperedges[hyperedgeInfo.ID]->nodes)
            {
                if (deg.find(node->id)!= deg.end())
                {
                    deg[node->id]--;
                }
                if (deg[node->id]==0)
                {
                    deg.erase(node->id);
                }
            }
            // 更新子图的信息
            for(auto & edge: EtoEMap[hyperedges[hyperedgeInfo.ID]])
            {
                // 这条边存在，并且这条边的k值等于当前的k值，才删除，因为不会影响到高层级的k
                if (edge.first->selected and edge.first->k == k)
                {
                    HyperedgeInfo incidentHyperedgeInfo = hyperedgeInfoMap[edge.first->id];
                    if(hyperedgeInfoSet.erase(incidentHyperedgeInfo))
                    {
                        HyperedgeInfo hyperedgeInfo;
                        hyperedgeInfo.selfSize = int(edge.first->nodes.size());
                        hyperedgeInfo.ID = edge.first->id;
                        for (auto &neiE: EtoEMap[edge.first])
                        {
                            if (neiE.first->selected)
                            {
                                hyperedgeInfo.incidentSizeOrder.insert(int(neiE.first->nodes.size()));
                                hyperedgeInfo.insectionOrder.insert(neiE.second);
                            }
                        }
                        hyperedgeInfoSet.insert(hyperedgeInfo);
                        hyperedgeInfoMap[hyperedgeInfo.ID] = hyperedgeInfo;
                    }
                }

            }
        }
        // 删除所有的边k
        for (auto & edge : newHyperEdge_k)
            edge->selected = false;
    }
    file.close();

    // 输出每个边的KS-Core信息
    file.open("/media/disk7T/liuyu/SCC/dataset/index/"+ datasetName + ".strongConnected.core");
    for (auto & edge : hyperedges) {
        file << edge->id << " " << edge->k << " " << edge->s << std::endl;
    }
    file.close();
}


#endif