#include <iostream>
#include <random>
#include <chrono>
#include <functional>
#include "Shoe.h"
#include "Game.h"

// keep:   g++ -std=c++17 -Iinclude src/Game.cpp src/Shoe.cpp src/HandEval.cpp src/Strategy.cpp src/Simulation.cpp src/TraceDemo.cpp -o trace_demo -pthread
int main(int argc, char** argv) {
    int rounds = 500;
    if (argc > 1) rounds = std::max(1, std::atoi(argv[1]));

    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    Shoe shoe(rng);

    double bankroll = 1000.0;
    const int betUnit = 25;

    auto betSizing = [](double tc) {
        int itc = static_cast<int>(std::floor(tc));
        if (itc <= 0) return 0;
        if (itc == 1) return 1;
        if (itc == 2) return 2;
        if (itc == 3) return 4;
        if (itc == 4) return 6;
        if (itc >= 5) return 8;
        return 0;
    };

    std::cout << "=== Blackjack Trace Demo for " << rounds << " rounds ===\n";
    for (int i = 1; i <= rounds; ++i) {
        std::cout << "\n-- Round " << i << " --\n";
        playGameTraced(shoe, bankroll, betUnit, betSizing);
    }

    std::cout << "\nFinal Bankroll: $" << bankroll << std::endl;
    return 0;
}


