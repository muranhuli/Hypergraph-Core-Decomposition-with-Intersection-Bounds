//
// Created by liuyu on 6/26/24.
//
#include <sstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <climits>
#include <unordered_set>
#include <map>

typedef std::pair<int, int> PII;

void getGraph(const std::string &filename, std::vector<std::vector<int>> &hyperEdge,
              std::unordered_map<int, std::vector<int>> &hyperNode)
{
    std::ifstream fin(filename, std::ios::in);
    int count = -1;
    while (true)
    {
        std::string str;
        getline(fin, str);
        if (str == "")
            break;
        std::istringstream ss(str);
        int tmp;
        std::vector<int> e;
        while (ss >> tmp)
        {
            if (find(e.begin(), e.end(), tmp) == e.end())
                e.push_back(tmp);
        }
        if (e.size() == 1)
            continue;
        count++;
        hyperEdge.push_back(e);
        for (auto &node: e)
            hyperNode[node].push_back(count);
    }
}

void load(std::string &path, std::unordered_map<int, std::pair<int, int>> &cocore)
{
    std::ifstream fin(path);
    if (!fin)
    {
        std::cerr << "open file failed" << std::endl;
        throw;
    }
    while (true)
    {
        std::string str;
        getline(fin, str);
        if (str == "")
            break;
        std::istringstream ss(str);
        int node, k, h;
        ss >> node;
        while (ss >> k >> h)
        {
            cocore[node] = std::make_pair(k, h);
        }
    }
}

void computeEcore(std::vector<std::vector<int>> &hyperEdge, std::unordered_map<int, std::pair<int, int>> &cocore,
                  std::unordered_map<int, PII> &ecore)
{
    for (size_t i = 0; i < hyperEdge.size(); i++)
    {
        int mink = INT_MAX;
        int minh = INT_MAX;
        for (auto &node: hyperEdge[i])
        {
            mink = std::min(mink, cocore[node].first);
        }
        for (auto &node: hyperEdge[i])
        {
            if (mink == cocore[node].first)
                minh = std::min(minh, cocore[node].second);
        }
        ecore[i] = std::make_pair(mink, minh);
    }
}

struct PairHash {
    size_t operator()(const std::pair<int, int>& p) const {
        auto hash1 = std::hash<int>{}(p.first);
        auto hash2 = std::hash<int>{}(p.second);
        // 使用位操作来混合两个哈希值
        return hash1 ^ (hash2 << 1);
    }
};

std::tuple<int,int,double> computeStatics(std::vector<std::vector<int>> &hyperEdge, std::vector<bool> &isExist)
{
    int edgeNum=0;
    int edgeLength = 0;
    std::unordered_set<int> nodeSet;
    for (size_t i = 0; i < isExist.size(); i++)
    {
        if (isExist[i])
        {
            edgeNum++;
            edgeLength += static_cast<int>(hyperEdge[i].size());
            for (auto &node: hyperEdge[i])
                nodeSet.insert(node);
        }
    }
    int nodeNum = static_cast<int>(nodeSet.size());
    double overlapping = 1.0 * edgeLength / nodeNum;
    return std::tuple<int,int,double>(edgeNum,nodeNum,overlapping);
}

int find(int x, const std::unordered_map<int, int> &fa)
{
    if (fa.at(x) == x)
        return x;
    else
        return find(fa.at(x), fa);
}

void merge(int i, int j, std::unordered_map<int, int> &fa)
{
    fa[i] = find(j, fa);
}

int connectedNum(std::vector<std::vector<int>> &hyperEdge, std::vector<bool> &isExist)
{
    std::unordered_map<int, int> fa;
    for (int i = 0; i < int(isExist.size()); i++)
    {
        if (isExist[i])
        {
            for (auto node : hyperEdge[i])
                fa[node] = node;
        }
    }

    for (int i = 0; i < int(isExist.size()); i++)
    {
        if (isExist[i])
        {
            for (int j = 0; j < int(hyperEdge[i].size()) - 1; j++)
            {
                if (fa[hyperEdge[i][j]] >= fa[hyperEdge[i][j + 1]])
                {
                    merge(hyperEdge[i][j], hyperEdge[i][j + 1], fa);
                }
                else
                {
                    merge(hyperEdge[i][j + 1], hyperEdge[i][j], fa);
                }
            }
        }
    }

    std::unordered_set<int> num;
    for (int i = 0; i < int(isExist.size()); i++)
    {
        if (isExist[i])
        {
            num.insert(hyperEdge[i][0]);
        }
    }
    return int(num.size());

}

void staticInformation(std::string dataSetName, std::vector<std::vector<int>> &hyperEdge, std::unordered_map<int, PII> &ecore)
{
    std::fstream fout("./result/kscore-1/"+dataSetName+".csv", std::ios::out);
    // 统计所有khcore的种类
    std::map<std::pair<int, int>, std::unordered_set<int>> khcore;
    for(auto it: ecore)
    {
        khcore[it.second].insert(it.first);
    }
    std::vector<bool> isExist(hyperEdge.size(), true);

    fout<<"k,h,connectedNum,edgeNum,nodeNum,overlapping"<<std::endl;
    for (auto &it: khcore)
    {
        std::tuple<int,int,double> r = computeStatics(hyperEdge,isExist);
        int num = connectedNum(hyperEdge,isExist);
        fout<<it.first.first<<","<<it.first.second<<","<<num<<","<<std::get<0>(r)<<","<<std::get<1>(r)<<","<<std::get<2>(r)<<std::endl;
        for (auto e:it.second)
            isExist[e]=false;
    }
}

int main(int argc, char *argv[])
{
    std::string dataSetName = argv[1];
    std::cout<<"Input "<<dataSetName<<std::endl;
    std::string filePath = "./dataset/dataset-1/";
    std::vector<std::vector<int>> hyperEdge;
    std::unordered_map<int, std::vector<int>> hyperNode;
    getGraph(filePath+dataSetName, hyperEdge, hyperNode);
    std::unordered_map<int, std::pair<int, int>> cocore;
    std::string indexPath = "./dataset/ks-1/" + dataSetName + "1.txt";
    // load(indexPath, cocore);
    std::unordered_map<int, PII> ecore;
    // computeEcore(hyperEdge, cocore, ecore);
    std::ifstream fin(indexPath);
    int a,b,c;
    while(fin>>a>>b>>c)
    {
        ecore[a]=std::make_pair(b,c);
    }
    std::vector<bool> isExist(hyperEdge.size(), true);
    staticInformation(dataSetName,hyperEdge, ecore);
    std::cout<<dataSetName<<" has finished!"<<std::endl;
    return 0;
}

