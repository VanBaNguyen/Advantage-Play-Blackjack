#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include "Simulation.h"

//g++ -std=c++17 -Iinclude src/Game.cpp src/Shoe.cpp src/HandEval.cpp src/Strategy.cpp src/Simulation.cpp main.cpp -o blackjack_sim -pthread
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
    const int startingBankroll = 10000;
    const int betUnit = 10;

    int playerWins = 0, dealerWins = 0, draws = 0;
    int surrenders = 0, splits = 0, doubles = 0;
    double finalBankroll = startingBankroll;

    // Configurable units-per-true-count. Return 0 to sit out.
    auto betSizing = [](double tc) {
        int itc = static_cast<int>(std::floor(tc));
        if (itc <= 0) return 0;    // sit out at TC <= 0
        if (itc == 1) return 1;    // TC 1 -> 1 unit
        if (itc == 2) return 2;    // TC 2 -> 2 units
        if (itc == 3) return 4;    // TC 3 -> 4 units
        if (itc == 4) return 6;    // TC 4 -> 6 units
        if (itc >= 5) return 8;    // TC 5+ -> 8 units (example)
        return 0;
    };

    simulateGames(totalGames,
                  playerWins, dealerWins, draws,
                  surrenders, splits, doubles,
                  startingBankroll,
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
