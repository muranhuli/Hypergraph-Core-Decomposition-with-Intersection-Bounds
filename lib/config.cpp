#ifndef CONFIG_H
#define CONFIG_H

#include <cstdio>
#include <vector>
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <thread>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <queue>
#include <functional>
#include <climits>
#include <stack>
#include <map>
#include <mutex>
using namespace std::chrono;

// ActorMovies Flickr Genre Github IMDB Teams TVTropes Wikipedia Walmart
std::vector<std::string> datasetNames = {"ActorMovies", "Flickr", "Genre", "Github", "IMDB", "Teams", "TVTropes", "Wikipedia", "Walmart",
                                         "Wikipedia-0.2", "Wikipedia-0.4", "Wikipedia-0.6", "Wikipedia-0.8"};

std::map<std::string, int> dataset_id = {
    {"ActorMovies", 0},
    {"Flickr", 1},
    {"Genre", 2},
    {"Github", 3},
    {"IMDB", 4},
    {"Teams", 5},
    {"TVTropes", 6},
    {"Wikipedia", 7},
    {"Walmart", 8},
    {"Wikipedia-0.2", 9},
    {"Wikipedia-0.4", 10},
    {"Wikipedia-0.6", 11},
    {"Wikipedia-0.8", 12}
};

template<typename T>
void bugs_output(T x) {
    std::cout << x << " ";
}
template<typename... Args>
void bugs(Args... args) {
    (..., bugs_output(args));
    std::cout << std::endl;
}
std::ofstream tout("time.txt", std::ios::app);

class Timer {
public:
    template<typename Func, typename... Args>
    static auto measure(const std::string& description, Func func, Args&&... args) {
        auto start = std::chrono::high_resolution_clock::now();
        
        if constexpr (std::is_same_v<std::invoke_result_t<Func, Args...>, void>) {
            // If the function returns void
            func(std::forward<Args>(args)...);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            tout <<" "<< description << "\t - Elapsed time: \t" << elapsed.count() * 1000 << "\tmirco seconds.\n";
        } else {
            // If the function returns a value
            auto result = func(std::forward<Args>(args)...);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            tout <<" "<< description << "\t - Elapsed time: \t" << elapsed.count() * 1000 << "\tmicro seconds.\n";
            return result;
        }
        std::cout << description << " - Done." << std::endl;
    }
};

#endif // CONFIG_H