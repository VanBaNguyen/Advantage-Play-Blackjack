#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include "Simulation.h"

//g++ -std=c++17 -Iinclude src/*.cpp main.cpp -o blackjack_sim -pthread
// int main() {
//     const int totalGames    = 20;
//     const int numThreads    = std::thread::hardware_concurrency() / 2;
//     const int perThread     = totalGames / numThreads;
//     const int startingBankroll = 1000; // Each thread starts with $1000
//     const int betUnit = 10;            // $10 per unit

//     std::vector<std::thread> threads;
//     std::vector<int> pw(numThreads), dw(numThreads), dr(numThreads),
//                      su(numThreads), sp(numThreads), db(numThreads);

//     // Here's the new part:
//     std::vector<double> finalBankrolls(numThreads, startingBankroll);

//     // Betting function (true count to units):
//     auto betSizing = [](double tc) {
//         if (tc < 1.0) return 1;
//         else return static_cast<int>(tc);
//     };

//     // Spawn threads
//     for (int i = 0; i < numThreads; ++i) {
//         threads.emplace_back(simulateGames,
//                              perThread,
//                              std::ref(pw[i]), std::ref(dw[i]),
//                              std::ref(dr[i]), std::ref(su[i]),
//                              std::ref(sp[i]), std::ref(db[i]),
//                              std::ref(finalBankrolls[i]), // add bankroll
//                              betUnit,
//                              betSizing);
//     }
//     for (auto& t : threads) t.join();

//     // aggregate & print...
//     int totalPW=0, totalDW=0, totalDR=0, totalSU=0, totalSP=0, totalDB=0;
//     double totalFinalBankroll = 0.0;
//     for (int i = 0; i < numThreads; ++i) {
//         totalPW += pw[i]; totalDW += dw[i]; totalDR += dr[i];
//         totalSU += su[i]; totalSP += sp[i]; totalDB += db[i];
//         totalFinalBankroll += finalBankrolls[i];
//     }

//     std::cout << "\nTotal games:  " << totalGames
//               << "\nPlayer wins:  " << totalPW
//               << "\nDealer wins:  " << totalDW
//               << "\nDraws:        " << totalDR
//               << "\n\nSurrenders:   " << totalSU
//               << "\nSplits:       " << totalSP
//               << "\nDoubles:      " << totalDB
//               << "\n\nTotal Final Bankroll: $" << totalFinalBankroll
//               << "\nAverage Final Bankroll per thread: $" << (totalFinalBankroll / numThreads)
//               << std::endl;

//     return 0;
// }

int main() {
    const int totalGames    = 10000;
    const int startingBankroll = 1000;
    const int betUnit = 10;

    int playerWins = 0, dealerWins = 0, draws = 0;
    int surrenders = 0, splits = 0, doubles = 0;
    double finalBankroll = startingBankroll;

    // Betting function (true count to units):
    auto betSizing = [](double tc) {
        if (tc < 1.0) return 1;
        else return static_cast<int>(tc);
    };

    simulateGames(totalGames,
                  playerWins, dealerWins, draws,
                  surrenders, splits, doubles,
                  finalBankroll, betUnit, betSizing);

    std::cout << "\nTotal games:  " << totalGames
              << "\nPlayer wins:  " << playerWins
              << "\nDealer wins:  " << dealerWins
              << "\nDraws:        " << draws
              << "\n\nSurrenders:   " << surrenders
              << "\nSplits:       " << splits
              << "\nDoubles:      " << doubles
              << "\n\nFinal Bankroll: $" << finalBankroll
              << std::endl;

    return 0;
}
