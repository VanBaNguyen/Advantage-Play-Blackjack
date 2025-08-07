#pragma once

#include <vector>
#include <random>

class Shoe {
public:
    Shoe(std::mt19937& rng);
    int draw();
    int getRunningCount() const;
    double getTrueCount() const;
    size_t getRemainingCards() const; // for tracing/visibility
    // Call at the start of each new round to reshuffle only between rounds
    void shuffleIfNeededForNewRound();
private:
    void refill();
    size_t remainingCards() const;
    size_t totalSize() const;
    bool needsRefillForNewRound() const;

    static constexpr int DECKS_IN_SHOE = 8;
    static constexpr int CARDS_PER_DECK = 52;
    static constexpr double RESHUFFLE_THRESHOLD = 0.2;

    std::mt19937& rng;
    std::vector<int> deck;
    size_t nextCardIndex = 0;
    int runningCount = 0;
    size_t cutCardIndex = 0; // position at which a new round should trigger a reshuffle
};
