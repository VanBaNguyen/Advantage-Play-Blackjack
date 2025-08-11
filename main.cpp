#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include "Simulation.h"

/*
g++ -std=c++17 -Iinclude src/Game.cpp src/Shoe.cpp src/HandEval.cpp src/Strategy.cpp src/Simulation.cpp main.cpp -o blackjack_sim -pthread
./blackjack_sim
*/


int main() {
    const int gamesPerSession   = 100000;
    const int numSessions = 1000;
    const double startingBankroll = 10000;
    const int betUnit = 10;

    // Configurable units-per-true-count. Return 0 to sit out.
    auto betSizing = [](double tc) {
        int itc = static_cast<int>(std::floor(tc));
        if (itc <= 0) return 0;    // sit out at TC <= 0
        if (itc == 1) return 20;
        if (itc == 2) return 30;
        if (itc == 3) return 40;
        if (itc == 4) return 50;
        if (itc >= 5) return 50;
        return 0;
    };
    // Monte Carlo sessions for EV and Risk of Ruin
    std::vector<double> finals(numSessions, 0.0);
    std::vector<bool> ruins(numSessions, false);

    unsigned int hc = std::thread::hardware_concurrency();
    if (hc == 0) hc = 4; // fallback
    int numWorkers = static_cast<int>(std::min<unsigned int>(numSessions, hc));
    std::atomic<int> nextIndex{0};
    std::vector<std::thread> workers;
    workers.reserve(numWorkers);
    for (int w = 0; w < numWorkers; ++w) {
        workers.emplace_back([&]() {
            while (true) {
                int i = nextIndex.fetch_add(1, std::memory_order_relaxed);
                if (i >= numSessions) break;
                double finalB = 0.0; bool r = false;
                simulateSingleSession(gamesPerSession, startingBankroll, finalB, r, betUnit, betSizing);
                finals[i] = finalB;
                ruins[i] = r;
            }
        });
    }
    for (auto& t : workers) t.join();

    double avgFinal = 0.0;
    int ruinCount = 0;
    for (int i = 0; i < numSessions; ++i) {
        avgFinal += finals[i];
        ruinCount += ruins[i] ? 1 : 0;
    }
    avgFinal /= numSessions;
    double evPerSession = avgFinal - startingBankroll;
    double evPerHand = evPerSession / gamesPerSession;
    double evPer100Hands = evPerHand * 100.0;
    double riskOfRuin = static_cast<double>(ruinCount) / numSessions;

    std::cout << "Sessions: " << numSessions
              << "\nGames per session: " << gamesPerSession
              << "\nStarting bankroll: $" << startingBankroll
              << "\nAverage final bankroll: $" << avgFinal
              << "\nEV per session: $" << evPerSession
              << "\nEV per 100 hands: $" << evPer100Hands;
    if (riskOfRuin == 0) {
        std::cout << "\nRisk of ruin: ~ <0%" << std::endl;
    } else {
        std::cout << "\nRisk of ruin: " << (riskOfRuin * 100.0) << "%" << std::endl;
    }

    return 0;
}
