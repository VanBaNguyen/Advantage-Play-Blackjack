#include "Strategy.h"

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
