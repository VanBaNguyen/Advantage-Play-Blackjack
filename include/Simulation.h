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

// Run a single session of N games. Returns final bankroll and whether bankroll hit 0 during play.
void simulateSingleSession(
    int games,
    double startingBankroll,
    double& finalBankroll,
    bool& ruined,
    int betUnit,
    std::function<int(double)> betSizing
);
