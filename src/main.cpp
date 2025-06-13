#include <iostream>
#include <vector>
#include <thread>
#include "Simulation.h"

int main() {
    /*
        g++ -std=c++17 -Iinclude src/*.cpp -o blackjack_sim -pthread
    */
    const int totalGames    = 100000;
    const int numThreads    = std::thread::hardware_concurrency() / 2;
    const int perThread     = totalGames / numThreads;

    std::vector<std::thread> threads;
    std::vector<int> pw(numThreads), dw(numThreads), dr(numThreads),
                     su(numThreads), sp(numThreads), db(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(simulateGames,
                             perThread,
                             std::ref(pw[i]), std::ref(dw[i]),
                             std::ref(dr[i]), std::ref(su[i]),
                             std::ref(sp[i]), std::ref(db[i]));
    }
    for (auto& t : threads) t.join();

    // aggregate & print...
    int totalPW=0, totalDW=0, totalDR=0, totalSU=0, totalSP=0, totalDB=0;
    for (int i = 0; i < numThreads; ++i) {
        totalPW += pw[i]; totalDW += dw[i]; totalDR += dr[i];
        totalSU += su[i]; totalSP += sp[i]; totalDB += db[i];
    }

    std::cout << "\nTotal games:  " << totalGames
              << "\nPlayer wins:  " << totalPW
              << "\nDealer wins:  " << totalDW
              << "\nDraws:        " << totalDR
              << "\n\nSurrenders:   " << totalSU
              << "\nSplits:       " << totalSP
              << "\nDoubles:      " << totalDB
              << std::endl;

    return 0;
}
