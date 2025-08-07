#pragma once
#include "Shoe.h"
#include <vector>
#include <functional>

// returns nothing, but increments the counters by reference
void playGame(
    Shoe& shoe,
    int& playerWins,
    int& dealerWins,
    int& draws,
    int& surrenderCounter,
    int& splitCounter,
    int& doubleCounter,
    double& bankroll,
    int betUnit,
    std::function<int(double)> betSizing
);

// A traced version that prints the details of a single round
void playGameTraced(
    Shoe& shoe,
    double& bankroll,
    int betUnit,
    std::function<int(double)> betSizing
);
