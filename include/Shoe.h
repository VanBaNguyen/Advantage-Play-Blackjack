#pragma once

#include <vector>
#include <random>

class Shoe {
public:
    Shoe(std::mt19937& rng);
    int draw();
    int getRunningCount() const;
    double getTrueCount() const;
private:
    void refill();
    size_t remainingCards() const;
    size_t totalSize() const;

    static constexpr int DECKS_IN_SHOE = 8;
    static constexpr int CARDS_PER_DECK = 52;
    static constexpr double RESHUFFLE_THRESHOLD = 0.2;

    std::mt19937& rng;
    std::vector<int> deck;
    size_t nextCardIndex = 0;
    int runningCount = 0;
};
