#include "Game.h"
#include "HandEval.h"
#include "Strategy.h"

void playGame(
    Shoe& shoe,
    int& playerWins,
    int& dealerWins,
    int& draws,
    int& surrenderCounter,
    int& splitCounter,
    int& doubleCounter
) {
    auto drawCard = [&]() { return shoe.draw(); };
    // use our evaluator / strategy helpers:
    using std::vector;

    vector<int> player = { drawCard(), drawCard() };
    vector<int> dealer = { drawCard(), drawCard() };
    int dealerUp = (dealer[0] == 1 ? 11 : dealer[0]);

    vector<vector<int>> hands;
    // split?
    if (player[0]==player[1] && shouldSplit(player[0], dealerUp)) {
        hands.push_back({player[0], drawCard()});
        hands.push_back({player[1], drawCard()});
        ++splitCounter;
    } else {
        hands.push_back(player);
    }

    int playerTotal = computeHandValue(player);
    // blackjack check
    if (player.size()==2 && playerTotal==21) {
        ++playerWins;
        return;
    }
    // surrender?
    if ((playerTotal==16 && (dealerUp>=9)) ||
        (playerTotal==15 && (dealerUp>=10)) ||
        (playerTotal==17 && dealerUp==11)) {
        ++surrenderCounter;
        ++dealerWins;
        return;
    }

    // each hand: maybe double, maybe hit/stand
    for (auto& h : hands) {
        bool didDouble = false;
        if (h.size()==2 && shouldDoubleDown(h, dealerUp)) {
            h.push_back(drawCard());
            ++doubleCounter;
            didDouble = true;
        }
        if (!didDouble) {
            while (true) {
                int val = computeHandValue(h);
                bool soft = isSoft(h);
                if (!soft) {
                    if (val>=17 || (val>=13&&val<=16&&dealerUp<=6) ||
                        (val==12&&dealerUp>=4&&dealerUp<=6))
                        break;
                } else {
                    if (val>=19 || (val==18&&dealerUp<=8))
                        break;
                }
                h.push_back(drawCard());
            }
        }
    }

    // dealer draws
    while (computeHandValue(dealer) < 17)
        dealer.push_back(drawCard());

    int dealerTotal = computeHandValue(dealer);
    // final compare
    for (auto& h : hands) {
        int v = computeHandValue(h);
        if (v>21)           ++dealerWins;
        else if (dealerTotal>21 || v>dealerTotal) ++playerWins;
        else if (dealerTotal>v)  ++dealerWins;
        else                   ++draws;
    }
}
