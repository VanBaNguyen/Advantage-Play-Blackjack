#include <iostream>
#include <random>
#include <chrono>
#include <functional>
#include "Shoe.h"
#include "Game.h"

int main(int argc, char** argv) {
    int rounds = 5;
    if (argc > 1) rounds = std::max(1, std::atoi(argv[1]));

    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    Shoe shoe(rng);

    double bankroll = 1000.0;
    const int betUnit = 10;

    auto betSizing = [](double tc) {
        if (tc < 1.0) return 1;
        return static_cast<int>(tc);
    };

    std::cout << "=== Blackjack Trace Demo for " << rounds << " rounds ===\n";
    for (int i = 1; i <= rounds; ++i) {
        std::cout << "\n-- Round " << i << " --\n";
        playGameTraced(shoe, bankroll, betUnit, betSizing);
    }

    std::cout << "\nFinal Bankroll: $" << bankroll << std::endl;
    return 0;
}


