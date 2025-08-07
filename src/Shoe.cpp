#include "Shoe.h"
#include <algorithm>

Shoe::Shoe(std::mt19937& rng) : rng(rng) {
    refill();
}

void Shoe::refill() {
    deck.clear();
    for (int d = 0; d < DECKS_IN_SHOE; ++d)
        for (int i = 1; i <= 13; ++i)
            for (int j = 0; j < 4; ++j)
                deck.push_back((i > 10) ? 10 : i);

    std::shuffle(deck.begin(), deck.end(), rng);
    nextCardIndex = 0;
    runningCount = 0;
    // Place cut card so that when we reach it, the next round will trigger a reshuffle
    // RESHUFFLE_THRESHOLD is fraction remaining (e.g., 0.2 means reshuffle with 20% left)
    // So cut position is totalSize() - threshold*totalSize() (e.g., 80% penetration)
    cutCardIndex = static_cast<size_t>(totalSize() - (RESHUFFLE_THRESHOLD * totalSize()));
}

int Shoe::draw() {
    int card = deck[nextCardIndex++];
    if (card >= 2 && card <= 6)       ++runningCount;   // 2-6: +1
    else if (card == 10 || card == 1) --runningCount;   // 10s and Aces: -1
    return card;
}

int Shoe::getRunningCount() const {
    return runningCount;
}

double Shoe::getTrueCount() const {
    double decksRem = static_cast<double>(remainingCards()) / CARDS_PER_DECK;
    if (decksRem < 0.1) decksRem = 0.1;
    return runningCount / decksRem;
}

size_t Shoe::remainingCards() const {
    return deck.size() - nextCardIndex;
}

size_t Shoe::totalSize() const {
    return DECKS_IN_SHOE * CARDS_PER_DECK;
}

size_t Shoe::getRemainingCards() const {
    return remainingCards();
}

bool Shoe::needsRefillForNewRound() const {
    // If we've dealt past the cut card, we should reshuffle before starting a new round
    return nextCardIndex >= cutCardIndex;
}

void Shoe::shuffleIfNeededForNewRound() {
    if (needsRefillForNewRound()) {
        refill();
    }
}
