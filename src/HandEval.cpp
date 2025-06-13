#include "HandEval.h"

int computeHandValue(const std::vector<int>& hand) {
    int total = 0, aces = 0;
    for (int card : hand) {
        total += card;
        if (card == 1) aces++;
    }
    while (total <= 11 && aces > 0) {
        total += 10;
        aces--;
    }
    return total;
}

bool isSoft(const std::vector<int>& hand) {
    int total = 0, aces = 0;
    for (int card : hand) {
        total += card;
        if (card == 1) aces++;
    }
    // if treating an ace as 11 keeps you ≤21, it’s a soft hand
    return (aces > 0 && total + 10 <= 21);
}
