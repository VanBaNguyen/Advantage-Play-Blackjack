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
    int& doubles,
    double startingBankroll,
    double& finalBankroll,
    int betUnit,
    std::function<int(double)> betSizing
) {
    double bankroll = startingBankroll;
    static std::atomic<uint64_t> uniq{0};
    auto seed = std::chrono::system_clock::now().time_since_epoch().count()
                ^ (static_cast<uint64_t>(std::hash<std::thread::id>{}(std::this_thread::get_id())) << 1)
                ^ uniq.fetch_add(1, std::memory_order_relaxed);
    std::mt19937 rng(seed);

    int pw=0, dw=0, dr=0, su=0, sp=0, db=0;
    Shoe shoe(rng);

    for (int i = 0; i < gamesPerThread; ++i) {
        playGame(shoe, pw, dw, dr, su, sp, db, bankroll, betUnit, betSizing);
    }

    playerWins  = pw;
    dealerWins  = dw;
    draws       = dr;
    surrenders  = su;
    splits      = sp;
    doubles     = db;
    finalBankroll = bankroll;
}

void simulateSingleSession(
    int games,
    double startingBankroll,
    double& finalBankroll,
    bool& ruined,
    int betUnit,
    std::function<int(double)> betSizing
) {
    ruined = false;
    double bankroll = startingBankroll;
    static std::atomic<uint64_t> uniq{0};
    auto seed = std::chrono::system_clock::now().time_since_epoch().count()
                ^ (static_cast<uint64_t>(std::hash<std::thread::id>{}(std::this_thread::get_id())) << 1)
                ^ uniq.fetch_add(1, std::memory_order_relaxed);
    std::mt19937 rng(seed);
    Shoe shoe(rng);

    int pw=0, dw=0, dr=0, su=0, sp=0, db=0;
    for (int i = 0; i < games; ++i) {
        if (bankroll <= 0.0) { ruined = true; break; }
        playGame(shoe, pw, dw, dr, su, sp, db, bankroll, betUnit, betSizing);
    }
    finalBankroll = bankroll;
}
