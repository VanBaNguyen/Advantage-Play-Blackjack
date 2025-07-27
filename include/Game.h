#pragma once
#include "Shoe.h"
#include <vector>

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
