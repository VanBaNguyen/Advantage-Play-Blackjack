#pragma once
#include <functional>

void simulateGames(
    int gamesPerThread,
    int& playerWins,
    int& dealerWins,
    int& draws,
    int& surrenders,
    int& splits,
    int& doubles,
    double startingBankroll,
    double& finalBankroll,
    int betUnit,
    std::function<int(double)> betSizing
);
