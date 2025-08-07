#pragma once
#include <vector>
#include <string>

bool shouldSplit(int pairValue, int dealerUpcard);

bool shouldDoubleDown(const std::vector<int>& hand, int dealerUpcard);

// Basic strategy decisions ("H","S","D","Ds"), empty if N/A
std::string best_move_soft(const std::vector<int>& hand, int dealerUpcard);
std::string best_move_hard(const std::vector<int>& hand, int dealerUpcard);

// Deviations based on true count ("Stand","Hit","Double","Split"), empty if none
std::string check_playing_deviations(const std::vector<int>& hand, int dealerUpcard, double trueCount);
