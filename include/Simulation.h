#pragma once
#include <functional> // <-- THIS LINE IS REQUIRED!

void simulateGames(
    int gamesPerThread,
    int& playerWins,
    int& dealerWins,
    int& draws,
    int& surrenders,
    int& splits,
    int& doubles,
    double& finalBankroll,
    int betUnit,
    std::function<int(double)> betSizing
);
