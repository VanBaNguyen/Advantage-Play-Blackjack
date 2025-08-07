#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include "Simulation.h"

/*
g++ -std=c++17 -Iinclude src/Game.cpp src/Shoe.cpp src/HandEval.cpp src/Strategy.cpp src/Simulation.cpp main.cpp -o blackjack_sim -pthread
./blackjack_sim
*/


int main() {
    const int gamesPerSession   = 1000;
    const int numSessions = 50000;
    const double startingBankroll = 5000;
    const int betUnit = 25;

    // Configurable units-per-true-count. Return 0 to sit out.
    auto betSizing = [](double tc) {
        int itc = static_cast<int>(std::floor(tc));
        if (itc <= 0) return 0;    // sit out at TC <= 0
        if (itc == 1) return 1;
        if (itc == 2) return 2;
        if (itc == 3) return 4;
        if (itc == 4) return 8;
        if (itc >= 5) return 12;
        return 0;
    };
    // Monte Carlo sessions for EV and Risk of Ruin
    std::vector<std::thread> threads;
    std::vector<double> finals(numSessions, 0.0);
    std::vector<bool> ruins(numSessions, false);

    for (int i = 0; i < numSessions; ++i) {
        threads.emplace_back([&, i]() {
            double finalB = 0.0; bool r = false;
            simulateSingleSession(gamesPerSession, startingBankroll, finalB, r, betUnit, betSizing);
            finals[i] = finalB;
            ruins[i] = r;
        });
    }
    for (auto& t : threads) t.join();

    double avgFinal = 0.0;
    int ruinCount = 0;
    for (int i = 0; i < numSessions; ++i) {
        avgFinal += finals[i];
        ruinCount += ruins[i] ? 1 : 0;
    }
    avgFinal /= numSessions;
    double evPerSession = avgFinal - startingBankroll;
    double riskOfRuin = static_cast<double>(ruinCount) / numSessions;

    if (riskOfRuin == 0) {
        std::cout << "Sessions: " << numSessions
        << "\nGames per session: " << gamesPerSession
        << "\nStarting bankroll: $" << startingBankroll
        << "\nAverage final bankroll: $" << avgFinal
        << "\nEV per session: $" << evPerSession
        << "\nRisk of ruin: ~ <0%" << std::endl;
    } else{ 
        std::cout << "Sessions: " << numSessions
        << "\nGames per session: " << gamesPerSession
        << "\nStarting bankroll: $" << startingBankroll
        << "\nAverage final bankroll: $" << avgFinal
        << "\nEV per session: $" << evPerSession
        << "\nRisk of ruin: " << (riskOfRuin * 100.0) << "%" << std::endl;
    }

    return 0;
}
