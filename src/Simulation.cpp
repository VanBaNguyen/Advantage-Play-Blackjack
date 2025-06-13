#include "Simulation.h"
#include "Game.h"
#include <thread>
#include <random>
#include <chrono>
#include <functional>

void simulateGames(
    int gamesPerThread,
    int& playerWins,
    int& dealerWins,
    int& draws,
    int& surrenders,
    int& splits,
    int& doubles
) {
    // seed per-thread RNG
    auto seed = std::chrono::system_clock::now().time_since_epoch().count()
                + std::hash<std::thread::id>{}(std::this_thread::get_id());
    std::mt19937 rng(seed);

    int pw=0, dw=0, dr=0, su=0, sp=0, db=0;
    Shoe shoe(rng);

    for (int i = 0; i < gamesPerThread; ++i) {
        playGame(shoe, pw, dw, dr, su, sp, db);
    }

    playerWins  = pw;
    dealerWins  = dw;
    draws       = dr;
    surrenders  = su;
    splits      = sp;
    doubles     = db;
}
