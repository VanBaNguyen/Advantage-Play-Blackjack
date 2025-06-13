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
}

int Shoe::draw() {
    if (remainingCards() <= RESHUFFLE_THRESHOLD * totalSize())
        refill();

    int card = deck[nextCardIndex++];
    if (card >= 2 && card <= 6)       ++runningCount;
    else if (card == 10 || card == 11) --runningCount;
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
