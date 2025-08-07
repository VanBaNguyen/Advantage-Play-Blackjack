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
    return (aces > 0 && total + 10 <= 21);
}
