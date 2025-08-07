#include "Strategy.h"
#include <algorithm>


bool shouldSplit(int pairValue, int dealerUpcard) {
    if (dealerUpcard == 1) dealerUpcard = 11;
    switch (pairValue) {
        case 1: return true;
        case 2: case 3: return dealerUpcard >= 2 && dealerUpcard <= 7;
        case 4: return dealerUpcard == 5 || dealerUpcard == 6;
        case 5: return false;
        case 6: return dealerUpcard >= 2 && dealerUpcard <= 6;
        case 7: return dealerUpcard <= 7;
        case 8: return true;
        case 9: return (dealerUpcard <= 6 || dealerUpcard == 8 || dealerUpcard == 9);
        case 10: return false;
        default: return false;
    }
}

static int card_str_to_int(const std::string& s) {
    if (s == "A") return 1;
    if (s == "T" || s == "J" || s == "Q" || s == "K") return 10;
    return std::max(2, std::min(10, std::stoi(s)));
}

static std::vector<int> hand_to_int_list(const std::vector<int>& hand) {
    return hand; // our engine uses ints already
}

std::string best_move_soft(const std::vector<int>& hand, int dealerUpcard) {
    auto values = hand_to_int_list(hand);
    if (dealerUpcard == 1) dealerUpcard = 11;
    int total = 0, aces = 0;
    for (int v : values) { total += v; if (v==1) ++aces; }
    bool soft = (aces > 0 && total + 10 <= 21);
    int value = soft ? total + 10 : total;

    if (!soft || value < 13 || value > 20) return "";

    static const std::string chart[21] = {""}; // placeholder for indexing
    // Build row-wise charts
    static const char* row20[10] = {"S","S","S","S","S","S","S","S","S","S"};
    static const char* row19[10] = {"S","S","S","S","Ds","S","S","S","S","S"};
    static const char* row18[10] = {"Ds","Ds","Ds","Ds","Ds","S","S","H","H","H"};
    static const char* row17[10] = {"H","D","D","D","D","H","H","H","H","H"};
    static const char* row16[10] = {"H","H","D","D","D","H","H","H","H","H"};
    static const char* row15[10] = {"H","H","D","D","D","H","H","H","H","H"};
    static const char* row14[10] = {"H","H","H","D","D","H","H","H","H","H"};
    static const char* row13[10] = {"H","H","H","D","D","H","H","H","H","H"};

    const char** table = nullptr;
    switch (value) {
        case 20: table = row20; break;
        case 19: table = row19; break;
        case 18: table = row18; break;
        case 17: table = row17; break;
        case 16: table = row16; break;
        case 15: table = row15; break;
        case 14: table = row14; break;
        case 13: table = row13; break;
        default: return "";
    }
    int idx = (dealerUpcard == 11) ? 9 : dealerUpcard - 2;
    return table[idx];
}

std::string best_move_hard(const std::vector<int>& hand, int dealerUpcard) {
    auto values = hand_to_int_list(hand);
    if (dealerUpcard == 1) dealerUpcard = 11;
    int total = 0, aces = 0;
    for (int v : values) { total += v; if (v==1) ++aces; }
    if (aces > 0 && total + 10 <= 21) return ""; // not hard

    if (total >= 17) return "S";
    if (total <= 8) return "H";

    static const char* row16[10] = {"S","S","S","S","S","H","H","H","H","H"};
    static const char* row15[10] = {"S","S","S","S","S","H","H","H","H","H"};
    static const char* row14[10] = {"S","S","S","S","S","H","H","H","H","H"};
    static const char* row13[10] = {"S","S","S","S","S","H","H","H","H","H"};
    static const char* row12[10] = {"H","H","S","S","S","H","H","H","H","H"};
    static const char* row11[10] = {"D","D","D","D","D","D","D","D","D","D"};
    static const char* row10[10] = {"D","D","D","D","D","D","D","D","H","H"};
    static const char* row9[10]  = {"H","D","D","D","D","H","H","H","H","H"};

    const char** table = nullptr;
    switch (total) {
        case 16: table = row16; break;
        case 15: table = row15; break;
        case 14: table = row14; break;
        case 13: table = row13; break;
        case 12: table = row12; break;
        case 11: table = row11; break;
        case 10: table = row10; break;
        case 9:  table = row9;  break;
        default: return "";
    }
    int idx = (dealerUpcard == 11) ? 9 : dealerUpcard - 2;
    return table[idx];
}

std::string check_playing_deviations(const std::vector<int>& hand, int dealerUpcard, double trueCount) {
    auto values = hand_to_int_list(hand);
    int total = 0;
    for (int v : values) total += v;
    bool isPair = values.size() == 2 && values[0] == values[1];

    auto is_pair_10s = [&](){
        return values.size()==2 && values[0]==10 && values[1]==10;
    };

    struct Rule { int pValType; int dCard; int tc; const char* move; int sense; };
    // pValType: actual total; -1 means pair of tens special
    // sense: +1 means ">="; -1 means "<="
    static const Rule rules[] = {
        {16, 9,   5,  "Stand",  +1},
        {16, 10,  0,  "Stand",  +1},
        {15, 10,  4,  "Stand",  +1},
        {13, 2,  -1,  "Stand",  +1},
        {13, 3,  -2,  "Stand",  +1},
        {12, 2,   4,  "Stand",  +1},
        {12, 3,   2,  "Stand",  +1},
        {12, 4,   0,  "Stand",  +1},
        {12, 5,  -1,  "Stand",  +1},
        {12, 6,  -1,  "Stand",  +1},
        {11, 11,  1,  "Double", +1},
        {10, 10,  4,  "Double", +1},
        {10, 11,  4,  "Double", +1},
        {9,  2,   1,  "Double", +1},
        {9,  7,   4,  "Double", +1},
        {-1, 5,   5,  "Split",  +1}, // pair of 10s vs 5
        {-1, 6,   5,  "Split",  +1}, // pair of 10s vs 6
    };

    int dUp = dealerUpcard;
    if (dUp == 1) dUp = 11;

    for (const auto& r : rules) {
        if (r.pValType == -1) {
            if (!is_pair_10s()) continue;
            if (dUp != r.dCard) continue;
            if ((r.sense > 0 && trueCount >= r.tc) || (r.sense < 0 && trueCount <= r.tc))
                return r.move;
        } else {
            if (total != r.pValType) continue;
            if (dUp != r.dCard) continue;
            if ((r.sense > 0 && trueCount >= r.tc) || (r.sense < 0 && trueCount <= r.tc))
                return r.move;
        }
    }
    return "";
}

bool shouldDoubleDown(const std::vector<int>& hand, int dealerUpcard) {
    if (dealerUpcard == 1) dealerUpcard = 11;
    int total = 0, aces = 0;
    for (int c : hand) { total += c; if (c == 1) aces++; }
    bool soft = (aces > 0 && total + 10 <= 21);
    int value = soft ? total + 10 : total;

    if (soft) {
        switch (value) {
            case 13: case 14: return dealerUpcard==5||dealerUpcard==6;
            case 15: case 16: return dealerUpcard>=4&&dealerUpcard<=6;
            case 17: return dealerUpcard>=3&&dealerUpcard<=6;
            case 18: return dealerUpcard>=2&&dealerUpcard<=6;
            case 19: return dealerUpcard==6;
            default: return false;
        }
    } else {
        switch (value) {
            case 9:  return dealerUpcard>=3&&dealerUpcard<=6;
            case 10: return dealerUpcard>=2&&dealerUpcard<=9;
            case 11: return true;
            default: return false;
        }
    }
}
