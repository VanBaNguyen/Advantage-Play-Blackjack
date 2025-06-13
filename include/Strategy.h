#pragma once
#include <vector>

// pairValue: the numeric value of both cards in the pair (1 for Ace, 2–10)
// dealerUpcard: 1 for Ace, 2–10 otherwise
bool shouldSplit(int pairValue, int dealerUpcard);

// hand: your two‐card hand; dealerUpcard same as above
bool shouldDoubleDown(const std::vector<int>& hand, int dealerUpcard);
