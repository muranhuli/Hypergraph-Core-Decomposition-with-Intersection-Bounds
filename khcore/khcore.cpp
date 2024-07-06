//
// Created by liuyu on 6/26/24.
//
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <chrono>
#include <map>
#include <climits>
#include <algorithm>
#include <random>

using namespace std;

typedef pair<int, int> PII;

void getGraph(const string &filename, vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode)
{
    ifstream fin(filename, ios::in);
    int count = -1;
    while (true)
    {
        string str;
        getline(fin, str);
        if (str == "")
            break;
        istringstream ss(str);
        int tmp;
        vector<int> e;
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

struct myCmp
{
    bool operator()(const pair<int, int> &a, const pair<int, int> &b) const
    {
        if (a.first == b.first)
            return false;
        else
        {
            if (a.second != b.second)
                return a.second < b.second;
            else
                return a.first < b.first;
        }
    }
};

struct cmp
{
    bool operator()(const PII &a, const PII &b) const
    {
        return a.second > b.second;
    }
};

struct hashPair
{
    size_t operator()(const pair<int, int> &p) const
    {
        return hash<int>()(p.first) & hash<int>()(p.second);
    }
};

struct pair_equal
{
    bool operator()(const pair<int, int> &a, const pair<int, int> &b) const
    {
        return a.first == b.first && a.second == b.second;
    }
};

void
hCoreDecomp(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core)
{
    core.clear();
    unordered_map<int, unordered_map<int, int>> nbr;
    set<pair<int, int>, myCmp> s;
    for (auto &it: hyperNode)
    {
        for (auto &edge: it.second)
        {
            for (auto &node: hyperEdge[edge])
            {
                nbr[it.first][node]++;
            }
        }
        nbr[it.first].erase(it.first);
        s.insert(make_pair(it.first, nbr[it.first].size()));
    }
    int k = 0;
    vector<bool> visitEdge(hyperEdge.size(), false);
    unordered_map<int, bool> visitNode;
    while (!s.empty())
    {
        pair<int, int> p = *s.begin();
        s.erase(s.begin());
        k = max(k, p.second);
        core[p.first] = k;
        visitNode[p.first] = true;
        for (auto &edge: hyperNode[p.first])
        {
            if (visitEdge[edge])
                continue;
            visitEdge[edge] = true;

            for (int i = 0; i < hyperEdge[edge].size(); i++)
            {
                int v = hyperEdge[edge][i];
                if (v == p.first)
                    continue;
                if (!visitNode[v])
                    s.erase(make_pair(v, nbr[v].size()));
                for (int j = 0; j < hyperEdge[edge].size(); j++)
                {
                    int u = hyperEdge[edge][j];
                    if (v == u)
                        continue;
                    nbr[v][u]--;
                    if (nbr[v][u] == 0)
                        nbr[v].erase(u);
                }
                if (!visitNode[v])
                    s.insert(make_pair(v, nbr[v].size()));
            }
        }
        nbr[p.first].clear();
    }
}

void
kCoreDecomp(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core)
{
    core.clear();
    set<pair<int, int>, myCmp> node_count;
    unordered_map<int, int> deg;
    vector<bool> visitEdge(hyperEdge.size(), false);
    unordered_map<int, bool> visitNode;
    for (auto &it: hyperNode)
    {
        deg[it.first] = it.second.size();
        node_count.insert(make_pair(it.first, deg[it.first]));
        visitNode[it.first] = false;
    }
    int K = 0;
    while (!node_count.empty())
    {
        pair<int, int> p = *node_count.begin();
        node_count.erase(node_count.begin());
        K = max(K, p.second);
        core[p.first] = K;
        visitNode[p.first] = true;
        for (auto &edge: hyperNode[p.first])
        {
            if (visitEdge[edge])
                continue;
            visitEdge[edge] = true;
            for (auto &node: hyperEdge[edge])
            {
                if (visitNode[node])
                    continue;
                if (node_count.erase(make_pair(node, deg[node])))
                {
                    deg[node]--;
                    node_count.insert(make_pair(node, deg[node]));
                }
            }
        }
    }
}

void save_nbr(string dataSet, unordered_map<int, unordered_map<int, int>>& nbr)
{
    ofstream fout("./result/khcore/index/"+dataSet+"_nbr.txt", ios::out);
    for(auto& it: nbr)
    {
        fout << it.first << " ";
        for(auto& itt: it.second)
        {
            fout << itt.first << " " << itt.second << " ";
        }
        fout << endl;
    }
    fout.close();
}

void khCoreDecomp(string dataSet, const vector<vector<int>> &hyperEdge, const unordered_map<int, vector<int>> &hyperNode,
                  unordered_map<int, pair<int, int>> &core)
{
    core.clear();
    //每次取边的度数最小的顶点
    set<pair<int, int>, myCmp> node_count;
    //计算kcore时，记录一个点边的度数
    unordered_map<int, int> degEdge;
    //计算k-core时，被删除的边
    vector<bool> visitEdge(hyperEdge.size(), false);
    //计算k-core时，被删除的点
    unordered_map<int, bool> visitNode;
    //计算h-core时，节点的邻居的种类和每一种有多少个
    unordered_map<int, unordered_map<int, int>> nbr;
    //计算h-core时，被删除的节点
    unordered_map<int, bool> hvisitNode;
    //每次取点的邻居数最小的顶点
    set<pair<int, int>, myCmp> s;
    //初始化degEdge, node_count, nbr, s
    for (auto &it: hyperNode)
    {
        for (auto &edge: it.second)
        {
            for (auto &node: hyperEdge.at(edge))
            {
                nbr[it.first][node]++;
            }
        }
        degEdge[it.first] = it.second.size();
        node_count.insert(make_pair(it.first, degEdge[it.first]));
        visitNode[it.first] = false;
        nbr[it.first].erase(it.first);
        s.insert(make_pair(it.first, nbr[it.first].size()));
    }

    // 将nbr存储
    save_nbr(dataSet, nbr);

    ofstream fout("./result/khcore/time/"+dataSet+".time");
    //以上初始化完成
    int K = 0;
    vector<int> visitEdgeTmp(hyperEdge.size(), false);
    auto nowTime = std::chrono::steady_clock::now();
    while (!node_count.empty())
    {
        pair<int, int> p = *node_count.begin();
        node_count.erase(node_count.begin());
        //如何k-core的k值小于目前节点的值，说明，上一轮的K计算完毕，进行h-core的计算
        if (K < p.second)
        {
            int k = 0;
            int count = 0;
            for (auto &it: core)
            {
                if (it.second.first == K)
                    count++;
            }
            vector<int> joinagain;
            while (!s.empty() && count != 0)
            {
                pair<int, int> pp = *s.begin();
                s.erase(s.begin());
                if (k!=0 and k!=max(k, pp.second))
                {
                    fout<<"k "<<K<<" h "<<k<<" "<<(std::chrono::steady_clock::now() - nowTime) / 1us << "us"<<endl;
                }
                k = max(k, pp.second);
                //如果节点不存在core值，说明没有计算k-core，则在本轮的h-core计算完毕后，需要将此节点即对应没有删除的边重新插入nbr
                if (core.find(pp.first) == core.end())
                {
                    joinagain.push_back(pp.first);
                } else if (core[pp.first].first == K)
                {
                    hvisitNode[pp.first] = true;
                    count--;
                    core[pp.first].second = k;
                }

                for (auto &edge: hyperNode.at(pp.first))
                {
                    if (visitEdgeTmp[edge])
                        continue;
                    visitEdgeTmp[edge] = true;

                    for (int i = 0; i < hyperEdge.at(edge).size(); i++)
                    {
                        int v = hyperEdge.at(edge).at(i);
                        if (v == pp.first)
                            continue;
                        s.erase(make_pair(v, nbr[v].size()));
                        for (int j = 0; j < hyperEdge.at(edge).size(); j++)
                        {

                            int u = hyperEdge.at(edge).at(j);
                            if (v == u)
                                continue;
                            //已经删除的节点不可访问，当前的节点不可访问
                            //因为有的节点在k-core计算时没有计算，但是h-core可能被删除，这时不算已经访问，因为需要判断当前节点不删除
                            if (nbr[v].find(u) != nbr[v].end())
                                nbr[v][u]--;
                            if (nbr[v][u] == 0)
                                nbr[v].erase(u);
                        }
                        // if (!hvisitNode[v] && v != pp.first)
                        s.insert(make_pair(v, nbr[v].size()));
                    }
                }
                nbr[pp.first].clear();
            }
            fout<<"k "<<K<<" h "<<k<<" "<<(std::chrono::steady_clock::now() - nowTime) / 1us << "us"<<endl;
            //将未被访问的节点重新插入回去
            for (auto &it: joinagain)
            {

                for (auto &edge: hyperNode.at(it))
                {
                    if (!visitEdgeTmp[edge])
                        continue;
                    bool flag = false;
                    for (auto &node: hyperEdge.at(edge))
                        if (hvisitNode[node])
                        {
                            flag = true;
                            break;
                        }
                    if (flag)
                        continue;
                    for (int i = 0; i < hyperEdge.at(edge).size(); i++)
                    {
                        int v = hyperEdge.at(edge).at(i);
                        for (int j = 0; j < hyperEdge.at(edge).size(); j++)
                        {

                            int u = hyperEdge.at(edge).at(j);
                            if (v == u)
                                continue;
                            s.erase(make_pair(v, nbr[v].size()));
                            nbr[v][u]++;
                            s.insert(make_pair(v, nbr[v].size()));
                        }
                    }
                    visitEdgeTmp[edge] = false;
                }
            }
            nowTime = std::chrono::steady_clock::now();
        }
        K = max(K, p.second);
        core[p.first].first = K;
        visitNode.at(p.first) = true;
        for (auto &edge: hyperNode.at(p.first))
        {
            if (visitEdge[edge])
                continue;
            visitEdge[edge] = true;
            for (auto &node: hyperEdge.at(edge))
            {
                if (visitNode[node])
                    continue;
                node_count.erase(make_pair(node, degEdge[node]));
                degEdge[node]--;
                node_count.insert(make_pair(node, degEdge[node]));
            }
        }
    }
    {
        int k = 0;
        int count = 0;
        for (auto &it: core)
        {
            if (it.second.first == K)
                count++;
        }
        while (!s.empty() && count != 0)
        {
            pair<int, int> p = *s.begin();
            s.erase(s.begin());
            k = max(k, p.second);
            if (core[p.first].first == K)
            {
                hvisitNode[p.first] = true;
                count--;
                core[p.first].second = k;
            }
            for (auto &edge: hyperNode.at(p.first))
            {
                if (visitEdgeTmp[edge])
                    continue;
                visitEdgeTmp[edge] = true;

                for (int i = 0; i < hyperEdge.at(edge).size(); i++)
                {
                    int v = hyperEdge.at(edge).at(i);
                    for (int j = 0; j < hyperEdge.at(edge).size(); j++)
                    {

                        int u = hyperEdge.at(edge).at(j);
                        if (v == u)
                            continue;
                        if (!hvisitNode[v] && v != p.first)
                            s.erase(make_pair(v, nbr[v].size()));
                        if (nbr[v].find(u) != nbr[v].end())
                            nbr[v][u]--;
                        if (nbr[v][u] == 0)
                            nbr[v].erase(u);
                        if (!hvisitNode[v] && v != p.first)
                            s.insert(make_pair(v, nbr[v].size()));
                    }
                }
            }
        }
        fout<<"k "<<K<<" h "<<k<<" "<<(std::chrono::steady_clock::now() - nowTime) / 1us << "us"<<endl;
    }
    fout.close();
}

void computeCore(const unordered_map<int, int> &core, map<int, int> &num)
{
    num.clear();
    for (auto &it: core)
    {
        num[it.second]++;
    }
}

void findHypergraphSubCore(const vector<vector<int>> &hyperEdge, const unordered_map<int, vector<int>> &hyperNode,
                           const unordered_map<int, pair<int, int>> &core, int u,
                           unordered_map<int, vector<int>> &preEdge, unordered_map<int, int> &cd)
{
    unordered_map<int, bool> visitNode;
    queue<int> Q;
    int k = core.at(u).first;
    Q.push(u);
    visitNode[u] = true;
    vector<int> visitEdge(hyperEdge.size(), false);
    while (!Q.empty())
    {
        int v = Q.front();
        Q.pop();
        cd[v] = 0;
        for (auto &edge: hyperNode.at(v))
        {
            int minK = INT_MAX;
            for (auto &w: hyperEdge[edge])
                minK = min(minK, core.at(w).first);
            if (minK == k)
            {
                cd[v]++;
                preEdge[v].push_back(edge);
                if (visitEdge[edge])
                    continue;
                visitEdge[edge] = true;
                for (auto &w: hyperEdge[edge])
                {
                    if (minK == core.at(w).first && visitNode.find(w) == visitNode.end())
                    {
                        Q.push(w);
                        visitNode[w] = true;
                    }
                }
            }
        }
    }
}

void hypergraph_dynamic_insert(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode,
                               unordered_map<int, pair<int, int>> &cocore, const vector<int> &e, int index)
{
    //更新图结构,插入超边
    //计算插入边的最小k值，以及在最小k值所对应的最小h值
    int k = INT_MAX;
    int h = INT_MAX;
    bool flag = false;
    for (auto &it: e)
    {
        hyperEdge[index].push_back(it);
        hyperNode[it].push_back(index);
        //或者kcore值为0时，删除cocore里面的点
        if (cocore.find(it) != cocore.end() && cocore[it].first != 0)
        {
            k = min(k, cocore[it].first);
        } else
        {
            cocore[it].first = 1;
            cocore[it].second = 1;
            k = 1;
            h = 1;
            flag = true;
        }
    }
    //计算最小的kh节点，和其对应的节点集合
    vector<int> minkh;
    for (auto &it: e)
    {
        if (cocore[it].first == k)
        {
            h = min(h, cocore[it].second);
            minkh.push_back(it);
        }
    }
    bool is_k_constant = true;
    unordered_map<int, int> cd;
    if (!flag) //flag成立就不需要更新节点的值了
    {
        //首先进行k值的更新
        unordered_map<int, vector<int>> preEdge;
        findHypergraphSubCore(hyperEdge, hyperNode, cocore, minkh[0], preEdge, cd);
        set<pair<int, int>, myCmp> mcd;
        unordered_map<int, int> deg;
        for (auto &node: cd)
        {
            if (cocore[node.first].first == k)
            {
                mcd.insert(make_pair(node.first, node.second));
                deg[node.first] = node.second;
            }
        }
        vector<bool> visitEdge(hyperEdge.size(), false);
        while (!mcd.empty() && (*mcd.begin()).second <= k)
        {
            pair<int, int> p = *mcd.begin();
            mcd.erase(mcd.begin());
            int v = p.first;
            for (auto &edge: preEdge[v])
            {
                if (visitEdge[edge])
                    continue;
                visitEdge[edge] = true;
                for (auto &node: hyperEdge[edge])
                {
                    if (deg.find(node) == deg.end() || deg.at(node) <= deg.at(v))
                        continue;
                    mcd.erase(make_pair(node, deg[node]));
                    deg[node]--;
                    mcd.insert(make_pair(node, deg[node]));
                }
            }
            deg.erase(v);
        }
        cd.clear();
        if (!mcd.empty())
            is_k_constant = false;
        while (!mcd.empty())
        {
            pair<int, int> p = *mcd.begin();
            cd.insert(p);
            mcd.erase(mcd.begin());
            cocore[p.first].first++;
        }
    }
    //如果有新插入的节点，那么重新进行coredecomp的范围为1
    if (flag)
    {
        k = 1;
    } else
    {
        //如果没有新插入的节点，且没有发生节点的升k，则coredecomp的范围为k，如果发生了节点的升k，则coredecomp的范围为k+1
        if (!is_k_constant)
            k++;
    }
    //确定新插入边e中k值最小的节点，它们h值可能增大的范围，即[h_LB,h_UB]
    //如果kcore发生了更新，则需要计算点的h值增大范围就在cd里面，否则，则在minkh中
    //变化的节点对应的邻居节点集合
    unordered_map<int, unordered_set<int>> nbr;
    if (cd.empty())
    {
        for (auto &it: minkh)
            nbr[it] = unordered_set<int>{};
    } else
    {
        for (auto &it: cd)
            nbr[it.first] = unordered_set<int>{};
    }
    auto minkhInEdge = [&cocore, &hyperEdge](int index) -> PII
    {
        int tmpk = INT_MAX, tmph = INT_MAX;
        for (auto &it: hyperEdge[index])
        {
            if (tmpk > cocore[it].first)
            {
                tmpk = cocore[it].first;
                tmph = cocore[it].second;
            } else if (tmpk == cocore[it].first)
            {
                tmph = min(tmph, cocore[it].second);
            }
        }
        return make_pair(tmpk, tmph);
    };
    auto computeMaxH = [&cocore](const unordered_set<int> &s) -> int
    {
        int maxH = 0;
        for (auto &it: s)
            maxH = max(maxH, cocore[it].second);
        vector<int> count(maxH + 1, 0);
        for (auto &it: s)
            count[cocore[it].second]++;
        for (int i = maxH - 1; i >= 0; i--)
            count[i] = count[i] + count[i + 1];
        int index = maxH;
        while (count[index] < index)
        {
            index--;
        }
        return index;
    };
    vector<PII> maxh; //每一个点对应的最大h
    for (auto &it: nbr)
    {
        for (auto &edge: hyperNode[it.first])
        {
            //*****错误!!!!!不加 .first
            if (cocore[it.first].first == minkhInEdge(edge).first)
            {
                for (auto &node: hyperEdge[edge])
                {
                    // if (cocore[node].second >= cocore[it.first].second)
                    it.second.insert(node);
                }
            }
        }
        it.second.erase(it.first);
        int tmp = computeMaxH(it.second);
        maxh.push_back(make_pair(it.first, tmp));
    }
    int h_LB = INT_MAX, h_UB = INT_MIN;
    for (int i = 0; i < maxh.size(); i++)
    {
        h_UB = max(h_UB, int(nbr[maxh[i].first].size()));
    }
    if (!is_k_constant)
    {
        for (int i = 0; i < maxh.size(); i++)
        {
            h_LB = min(h_LB, maxh[i].second);
        }
        //这里需要确定当k发生升级时,新出现点,所在的边中所有的点最小的h-LB
        //有符号减无符号需要注意
        // h_LB = ((h_LB - int(minkh.size())) >= 0 ? (h_LB - int(minkh.size())) : 0);
        h_LB = 0;
    } else
        h_LB = h;
    // h_LB=0;
    // cout << "h_LB = " << h_LB << endl;
    // cout << "h_UB = " << h_UB << endl;
    vector<bool> isDelete(hyperEdge.size(), false);

    unordered_map<int, bool> visitNode;
    for (auto &it: hyperNode)
    {
        if (cocore[it.first].first < k)
        {
            visitNode[it.first] = true;
            for (auto &edge: it.second)
                isDelete[edge] = true;
        } else if (cocore[it.first].first == k && cocore[it.first].second < h_LB)
        {
            visitNode[it.first] = true;
            for (auto &edge: it.second)
                isDelete[edge] = true;
        }
    }
    //运行hcoreDecomp分解算法
    unordered_map<int, unordered_map<int, int>> hnbr;
    set<pair<int, int>, myCmp> s;
    for (auto &it: hyperNode)
    {
        if (visitNode[it.first])
            continue;
        for (auto &edge: it.second)
        {
            if (isDelete[edge])
                continue;
            for (auto &node: hyperEdge[edge])
            {
                hnbr[it.first][node]++;
            }
        }
        hnbr[it.first].erase(it.first);
        s.insert(make_pair(it.first, hnbr[it.first].size()));
    }
    h = h_LB;
    while (!s.empty())
    {
        pair<int, int> p = *s.begin();
        s.erase(s.begin());
        h = max(h, p.second);
        if (h > h_UB)
            break;
        if (cocore[p.first].first == k)
        {
            cocore[p.first].second = h;
        }
        visitNode[p.first] = true;
        for (auto &edge: hyperNode[p.first])
        {
            if (isDelete[edge])
                continue;
            isDelete[edge] = true;

            for (int i = 0; i < hyperEdge[edge].size(); i++)
            {
                int v = hyperEdge[edge][i];
                if (v == p.first)
                    continue;
                if (!visitNode[v])
                    s.erase(make_pair(v, hnbr[v].size()));
                for (int j = 0; j < hyperEdge[edge].size(); j++)
                {
                    int u = hyperEdge[edge][j];
                    if (v == u)
                        continue;
                    if (hnbr[v].find(u) != hnbr[v].end())
                        hnbr[v][u]--;
                    if (hnbr[v][u] == 0)
                        hnbr[v].erase(u);
                }
                if (!visitNode[v])
                    s.insert(make_pair(v, hnbr[v].size()));
            }
        }
        hnbr[p.first].clear();
    }
}

void hypergraph_dynamic_erase(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode,
                              unordered_map<int, pair<int, int>> &cocore, int index)
{
    //更新图结构,插入超边
    //计算插入边的最小k值，以及在最小k值所对应的最小h值
    int k = INT_MAX;
    int h = INT_MAX;
    vector<int> e(hyperEdge[index]);
    hyperEdge[index] = vector<int>();
    for (auto &it: e)
    {
        hyperNode[it].erase(find(hyperNode[it].begin(), hyperNode[it].end(), index));
        k = min(k, cocore[it].first);
    }
    //计算最小的kh节点，和其对应的节点集合
    vector<int> minkh;
    for (auto &it: e)
    {
        if (cocore[it].first == k)
        {
            h = min(h, cocore[it].second);
            minkh.push_back(it);
        }
    }
    bool is_k_constant = true;
    unordered_map<int, int> cd;
    //首先进行k值的更新
    unordered_map<int, vector<int>> preEdge;
    for (auto &it: minkh)
    {
        unordered_map<int, int> cdd;
        unordered_map<int, vector<int>> preEdgee;
        findHypergraphSubCore(hyperEdge, hyperNode, cocore, it, preEdgee, cdd);
        for (auto &t1: cdd)
        {
            if (cd.find(t1.first) == cd.end())
                cd[t1.first] = t1.second;
        }
        for (auto &t2: preEdgee)
        {
            if (preEdge.find(t2.first) == preEdge.end())
                preEdge[t2.first] = t2.second;
        }
    }
    set<pair<int, int>, myCmp> mcd;
    unordered_map<int, int> deg;
    for (auto &node: cd)
    {
        if (cocore[node.first].first == k)
        {
            mcd.insert(make_pair(node.first, node.second));
            deg[node.first] = node.second;
        }
    }
    vector<bool> visitEdge(hyperEdge.size(), false);
    cd.clear();
    while (!mcd.empty() && (*mcd.begin()).second < k)
    {
        pair<int, int> p = *mcd.begin();
        mcd.erase(mcd.begin());
        int v = p.first;
        cd[v] = 0;
        for (auto &edge: preEdge[v])
        {
            if (visitEdge[edge])
                continue;
            visitEdge[edge] = true;
            for (auto &node: hyperEdge[edge])
            {
                if (deg.find(node) == deg.end() || deg.at(node) <= deg.at(v))
                    continue;
                mcd.erase(make_pair(node, deg[node]));
                deg[node]--;
                mcd.insert(make_pair(node, deg[node]));
            }
        }
        deg.erase(v);
    }
    for (auto &it: cd)
    {
        is_k_constant = false;
        cocore[it.first].first--;
    }
    //如果有新插入的节点，那么重新进行coredecomp的范围为1
    //如果没有新插入的节点，且没有发生节点的升k，则coredecomp的范围为k，如果发生了节点的升k，则coredecomp的范围为k+1
    // if (!is_k_constant)
    //     k--;
    //确定新插入边e中k值最小的节点，它们h值可能增大的范围，即[h_LB,h_UB]
    //如果kcore发生了更新，则需要计算点的h值增大范围就在cd里面，否则，则在minkh中
    //变化的节点对应的邻居节点集合

    if (true)
    {
        unordered_map<int, unordered_set<int>> nbr;
        for (auto &it: minkh)
        {
            if (cocore[it].first == k)
            {
                nbr[it] = unordered_set<int>{};
            }
        }
        auto minkhInEdge = [&cocore, &hyperEdge](int index) -> PII
        {
            int tmpk = INT_MAX, tmph = INT_MAX;
            for (auto &it: hyperEdge[index])
            {
                if (tmpk > cocore[it].first)
                {
                    tmpk = cocore[it].first;
                    tmph = cocore[it].second;
                } else if (tmpk == cocore[it].first)
                {
                    tmph = min(tmph, cocore[it].second);
                }
            }
            return make_pair(tmpk, tmph);
        };
        auto computeMaxH = [&cocore](const unordered_set<int> &s) -> int
        {
            int maxH = 0;
            for (auto &it: s)
                maxH = max(maxH, cocore[it].second);
            vector<int> count(maxH + 1, 0);
            for (auto &it: s)
                count[cocore[it].second]++;
            for (int i = maxH - 1; i >= 0; i--)
                count[i] = count[i] + count[i + 1];
            int index = maxH;
            while (count[index] < index)
            {
                index--;
            }
            return index;
        };

        vector<PII> maxh; //每一个点对应的最大h
        for (auto &it: nbr)
        {
            for (auto &edge: hyperNode[it.first])
            {
                if (cocore[it.first].first == minkhInEdge(edge).first)
                {
                    for (auto &node: hyperEdge[edge])
                    {
                        it.second.insert(node);
                    }
                }
            }
            it.second.erase(it.first);
            int tmp = computeMaxH(it.second);
            maxh.push_back(make_pair(it.first, tmp));
        }
        int h_LB = INT_MAX, h_UB = INT_MAX;
        for (int i = 0; i < maxh.size(); i++)
        {
            //一个点下降,会导致这个点所在所有边均下降,则下降的边中的最小值才是hlb
            h_LB = min(h_LB, maxh[i].second);
            h_UB = min(h_UB, cocore[maxh[i].first].second);
            // h_UB = max(h_UB, int(nbr[maxh[i].first].size()));
        }
        h_LB = 0;

        // h_UB = h;
        // cout << "h_LB = " << h_LB << endl;
        // cout << "h_UB = " << h_UB << endl;
        vector<bool> isDelete(hyperEdge.size(), false);

        unordered_map<int, bool> visitNode;
        for (auto &it: hyperNode)
        {
            if (cocore[it.first].first < k)
            {
                visitNode[it.first] = true;
                for (auto &edge: it.second)
                    isDelete[edge] = true;
            } else if (cocore[it.first].first == k && cocore[it.first].second < h_LB)
            {
                visitNode[it.first] = true;
                for (auto &edge: it.second)
                    isDelete[edge] = true;
            }
        }
        //运行hcoreDecomp分解算法
        unordered_map<int, unordered_map<int, int>> hnbr;
        set<pair<int, int>, myCmp> s;
        for (auto &it: hyperNode)
        {
            if (visitNode[it.first])
                continue;
            for (auto &edge: it.second)
            {
                if (isDelete[edge])
                    continue;
                for (auto &node: hyperEdge[edge])
                {
                    hnbr[it.first][node]++;
                }
            }
            hnbr[it.first].erase(it.first);
            s.insert(make_pair(it.first, hnbr[it.first].size()));
        }
        h = h_LB;
        while (!s.empty())
        {
            pair<int, int> p = *s.begin();
            s.erase(s.begin());
            h = max(h, p.second);
            if (h > h_UB)
            {
                break;
            }

            if (cocore[p.first].first == k)
            {
                cocore[p.first].second = h;
            }
            visitNode[p.first] = true;
            for (auto &edge: hyperNode[p.first])
            {
                if (isDelete[edge])
                    continue;
                isDelete[edge] = true;

                for (int i = 0; i < hyperEdge[edge].size(); i++)
                {
                    int v = hyperEdge[edge][i];
                    if (v == p.first)
                        continue;
                    if (!visitNode[v])
                        s.erase(make_pair(v, hnbr[v].size()));
                    for (int j = 0; j < hyperEdge[edge].size(); j++)
                    {
                        int u = hyperEdge[edge][j];
                        if (v == u)
                            continue;
                        if (hnbr[v].find(u) != hnbr[v].end())
                            hnbr[v][u]--;
                        if (hnbr[v][u] == 0)
                            hnbr[v].erase(u);
                    }
                    if (!visitNode[v])
                        s.insert(make_pair(v, hnbr[v].size()));
                }
            }
            hnbr[p.first].clear();
        }
    }

    if (!cd.empty())
    {
        k--;
        unordered_map<int, unordered_set<int>> nbr;
        for (auto &it: cd)
            nbr[it.first] = unordered_set<int>{};
        auto minkhInEdge = [&cocore, &hyperEdge](int index) -> PII
        {
            int tmpk = INT_MAX, tmph = INT_MAX;
            for (auto &it: hyperEdge[index])
            {
                if (tmpk > cocore[it].first)
                {
                    tmpk = cocore[it].first;
                    tmph = cocore[it].second;
                } else if (tmpk == cocore[it].first)
                {
                    tmph = min(tmph, cocore[it].second);
                }
            }
            return make_pair(tmpk, tmph);
        };
        auto computeMaxH = [&cocore](const unordered_set<int> &s) -> int
        {
            int maxH = 0;
            for (auto &it: s)
                maxH = max(maxH, cocore[it].second);
            vector<int> count(maxH + 1, 0);
            for (auto &it: s)
                count[cocore[it].second]++;
            for (int i = maxH - 1; i >= 0; i--)
                count[i] = count[i] + count[i + 1];
            int index = maxH;
            while (count[index] < index)
            {
                index--;
            }
            return index;
        };
        vector<PII> maxh; //每一个点对应的最大h
        for (auto &it: nbr)
        {
            for (auto &edge: hyperNode[it.first])
            {
                if (cocore[it.first].first == minkhInEdge(edge).first)
                {
                    for (auto &node: hyperEdge[edge])
                    {
                        // if (cocore[node].second >= cocore[it.first].second)
                        it.second.insert(node);
                    }
                }
            }
            it.second.erase(it.first);
            int tmp = computeMaxH(it.second);

            maxh.push_back(make_pair(it.first, tmp));
        }
        int h_LB = INT_MAX, h_UB = INT_MIN;
        for (int i = 0; i < maxh.size(); i++)
        {
            h_LB = min(h_LB, maxh[i].second);
            h_UB = max(h_UB, int(nbr[maxh[i].first].size()));
        }
        h_LB = 0;
        // cout << "cdh_LB = " << h_LB << endl;
        // // h_UB = h;
        // cout << "cdh_UB = " << h_UB << endl;
        vector<bool> isDelete(hyperEdge.size(), false);

        unordered_map<int, bool> visitNode;
        for (auto &it: hyperNode)
        {
            if (cocore[it.first].first < k)
            {
                visitNode[it.first] = true;
                for (auto &edge: it.second)
                    isDelete[edge] = true;
            } else if (cocore[it.first].first == k && cocore[it.first].second < h_LB)
            {
                visitNode[it.first] = true;
                for (auto &edge: it.second)
                    isDelete[edge] = true;
            }
        }
        //运行hcoreDecomp分解算法
        unordered_map<int, unordered_map<int, int>> hnbr;
        set<pair<int, int>, myCmp> s;
        for (auto &it: hyperNode)
        {
            if (visitNode[it.first])
                continue;
            for (auto &edge: it.second)
            {
                if (isDelete[edge])
                    continue;
                for (auto &node: hyperEdge[edge])
                {
                    hnbr[it.first][node]++;
                }
            }
            hnbr[it.first].erase(it.first);
            s.insert(make_pair(it.first, hnbr[it.first].size()));
        }
        h = h_LB;
        while (!s.empty())
        {
            pair<int, int> p = *s.begin();
            s.erase(s.begin());
            h = max(h, p.second);
            if (h > h_UB)
                break;
            if (cocore[p.first].first == k)
            {
                cocore[p.first].second = h;
            }
            visitNode[p.first] = true;
            for (auto &edge: hyperNode[p.first])
            {
                if (isDelete[edge])
                    continue;
                isDelete[edge] = true;

                for (int i = 0; i < hyperEdge[edge].size(); i++)
                {
                    int v = hyperEdge[edge][i];
                    if (v == p.first)
                        continue;
                    if (!visitNode[v])
                        s.erase(make_pair(v, hnbr[v].size()));
                    for (int j = 0; j < hyperEdge[edge].size(); j++)
                    {
                        int u = hyperEdge[edge][j];
                        if (v == u)
                            continue;
                        if (hnbr[v].find(u) != hnbr[v].end())
                        {
                            hnbr[v][u]--;
                            if (hnbr[v][u] == 0)
                                hnbr[v].erase(u);
                        }
                    }
                    if (!visitNode[v])
                        s.insert(make_pair(v, hnbr[v].size()));
                }
            }
            hnbr[p.first].clear();
        }
    }
}

void save_cocore(string dataSetName, unordered_map<int, pair<int, int>> &cocore)
{
    string fileName = "./result/khcore/index/" + dataSetName + ".cocore";
    ofstream out(fileName, ios::out);
    for (auto &it: cocore)
    {
        out << it.first << " " << it.second.first << " " << it.second.second << endl;
    }
    out.close();
}





int main(int argc, char **argv)
{
    string datasetName = argv[1];
    string filePath = "";
    string fileName = filePath + datasetName;
    vector<vector<int>> hyperEdge;
    unordered_map<int, vector<int>> hyperNode;
    getGraph(fileName, hyperEdge, hyperNode);
    cout<<"Input "<<datasetName<<" "<< "edge is " << hyperEdge.size() << "  node is " << hyperNode.size() << endl;

    unordered_map<int, pair<int, int>> cocore;
    khCoreDecomp(datasetName, hyperEdge, hyperNode, cocore);
    save_cocore(datasetName, cocore);
    cout<<datasetName<<" is finished!"<<endl;
}