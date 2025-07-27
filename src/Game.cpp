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
    int& doubleCounter,
    double& bankroll,
    int betUnit,
    std::function<int(double)> betSizing
) {
    // Determine bet for this round
    double trueCount = shoe.getTrueCount();
    int unitsToBet = betSizing(trueCount);
    int bet = std::min(betUnit * unitsToBet, static_cast<int>(bankroll));
    if (bet <= 0) return;

    auto drawCard = [&]() { return shoe.draw(); };
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
    if (player.size() == 2 && playerTotal == 21) {
        ++playerWins;
        bankroll += 1.5 * bet;
        return;
    }
    // surrender?
    if ((playerTotal==16 && (dealerUp>=9)) ||
        (playerTotal==15 && (dealerUp>=10)) ||
        (playerTotal==17 && dealerUp==11)) {
        ++surrenderCounter;
        ++dealerWins;
        // Normally, you lose half your bet on surrender:
        bankroll -= 0.5 * bet;
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

    // For each hand played (splits or not), adjust bankroll **for each**
for (auto& h : hands) {
    int v = computeHandValue(h);
    int handBet = bet; // for splits, each hand should be a full bet
    bool didDouble = (h.size() == 3 && shouldDoubleDown(h, dealerUp)); // crude, but works for 1-card double
    if (didDouble) handBet *= 2;

    if (v > 21) {
        ++dealerWins;
        bankroll -= handBet;
    } else if (dealerTotal > 21 || v > dealerTotal) {
        ++playerWins;
        bankroll += handBet;
    } else if (dealerTotal > v) {
        ++dealerWins;
        bankroll -= handBet;
    } else {
        ++draws;
        // Push: bankroll unchanged
    }
}
}
