#pragma once
#include <vector>
#include <string>

// pairValue: the numeric value of both cards in the pair (1 for Ace, 2–10)
// dealerUpcard: 1 for Ace, 2–10 otherwise
bool shouldSplit(int pairValue, int dealerUpcard);

// hand: your two‐card hand; dealerUpcard same as above
bool shouldDoubleDown(const std::vector<int>& hand, int dealerUpcard);

// New: basic strategy recommendations for hard/soft totals
// Returns one of: "H" (hit), "S" (stand), "D" (double), "Ds" (double otherwise stand), or empty if not applicable
std::string best_move_soft(const std::vector<int>& hand, int dealerUpcard);
std::string best_move_hard(const std::vector<int>& hand, int dealerUpcard);

// New: playing deviations based on true count
// Returns one of: "Stand", "Hit", "Double", "Split" or empty if no deviation
std::string check_playing_deviations(const std::vector<int>& hand, int dealerUpcard, double trueCount);
