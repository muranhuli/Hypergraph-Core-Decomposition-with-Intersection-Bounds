#ifndef UNION_FIND_H
#define UNION_FIND_H
#include <vector>
#include <algorithm>
#include <numeric>
struct dsu {
    std::vector<int> pa, size;
    dsu(int size_) : pa(size_ + 1), size(size_ + 1, 1) {
        iota(pa.begin(), pa.end(), 0);
    }
    void unite(int x, int y) {
        x = find(x), y = find(y);
        if (x == y) return;
        if (size[x] < size[y]) std::swap(x, y);
        pa[y] = x;
        size[x] += size[y];
    }
    int find(int x) {
        return pa[x] == x ? x : pa[x] = find(pa[x]);
    }
};
#endif