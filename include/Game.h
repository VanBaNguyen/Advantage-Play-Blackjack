#pragma once
#include "Shoe.h"
#include <vector>
#include <functional>

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

void playGameTraced(
    Shoe& shoe,
    double& bankroll,
    int betUnit,
    std::function<int(double)> betSizing
);
