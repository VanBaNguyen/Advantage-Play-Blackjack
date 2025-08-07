#include "Game.h"
#include "HandEval.h"
#include "Strategy.h"
#include <iostream>

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
    // Reshuffle only between rounds if we've reached the cut card
    shoe.shuffleIfNeededForNewRound();

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
    std::vector<bool> handDoubled; // track which split hand doubled
    // split? consider deviations too (e.g., pair of 10s at high counts)
    bool isPair = (player[0] == player[1]);
    std::string splitDev = check_playing_deviations(player, dealerUp, trueCount);
    if (isPair && (shouldSplit(player[0], dealerUp) || splitDev == "Split")) {
        hands.push_back({player[0], drawCard()});
        hands.push_back({player[1], drawCard()});
        handDoubled.push_back(false);
        handDoubled.push_back(false);
        ++splitCounter;
    } else {
        hands.push_back(player);
        handDoubled.push_back(false);
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

    // each hand: decide via basic strategy + deviations
    for (size_t i = 0; i < hands.size(); ++i) {
        auto& h = hands[i];
        bool completed = false;
        while (!completed) {
            int val = computeHandValue(h);
            if (val >= 21) break; // blackjack/bust

            // Base move
            std::string move = best_move_soft(h, dealerUp);
            if (move.empty()) move = best_move_hard(h, dealerUp);

            // Deviations by true count
            std::string dev = check_playing_deviations(h, dealerUp, shoe.getTrueCount());
            if (!dev.empty()) {
                if (dev == "Stand") move = "S";
                else if (dev == "Hit") move = "H";
                else if (dev == "Double") move = "D";
                else if (dev == "Split") {
                    // Only relevant on initial hand; skip here
                }
            }

            // Apply move
            if (move == "S") {
                break;
            } else if (move == "D" || move == "Ds") {
                if (h.size() == 2) {
                    h.push_back(drawCard());
                    ++doubleCounter;
                    handDoubled[i] = true;
                }
                // Ds stands if cannot double; D hits if cannot double
                if (move == "D" && h.size() != 3) {
                    h.push_back(drawCard());
                    continue;
                }
                completed = true; // doubled -> stop
            } else { // "H"
                h.push_back(drawCard());
            }
        }
    }

    // dealer draws
    while (computeHandValue(dealer) < 17)
        dealer.push_back(drawCard());

    int dealerTotal = computeHandValue(dealer);

    // For each hand played (splits or not), adjust bankroll per hand
    for (size_t i = 0; i < hands.size(); ++i) {
        auto& h = hands[i];
        int v = computeHandValue(h);
        int handBet = bet; // for splits, each hand should be a full bet
        if (handDoubled[i]) handBet *= 2;

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

void playGameTraced(
    Shoe& shoe,
    double& bankroll,
    int betUnit,
    std::function<int(double)> betSizing
) {
    using std::cout; using std::endl; using std::vector;
    shoe.shuffleIfNeededForNewRound();

    double trueCount = shoe.getTrueCount();
    int unitsToBet = betSizing(trueCount);
    int bet = std::min(betUnit * unitsToBet, static_cast<int>(bankroll));
    if (bet <= 0) {
        cout << "Insufficient bankroll to bet. Bankroll: $" << bankroll << endl;
        return;
    }

    auto drawCard = [&]() { return shoe.draw(); };
    auto cardStr = [](int c){ return c==1? std::string("A"): (c==10? std::string("T"): std::to_string(c)); };

    vector<int> player = { drawCard(), drawCard() };
    vector<int> dealer = { drawCard(), drawCard() };
    int dealerUp = (dealer[0] == 1 ? 11 : dealer[0]);

    cout << "Bet: $" << bet << "  True Count: " << trueCount
         << "  Running Count: " << shoe.getRunningCount()
         << "  Cards Remaining: " << shoe.getRemainingCards() << endl;
    cout << "Player: [" << cardStr(player[0]) << ", " << cardStr(player[1]) << "] (" << computeHandValue(player) << ")" << endl;
    cout << "Dealer: [" << cardStr(dealer[0]) << ", ?]" << endl;

    vector<vector<int>> hands;
    std::vector<bool> handDoubled;
    if (player[0]==player[1] && shouldSplit(player[0], dealerUp)) {
        hands.push_back({player[0], drawCard()});
        hands.push_back({player[1], drawCard()});
        handDoubled.push_back(false);
        handDoubled.push_back(false);
        cout << "Action: Split" << endl;
    } else {
        hands.push_back(player);
        handDoubled.push_back(false);
    }

    int playerTotal = computeHandValue(player);
    if (player.size() == 2 && playerTotal == 21) {
        cout << "Player Blackjack! Payout +$" << 1.5 * bet << endl;
        bankroll += 1.5 * bet;
        return;
    }

    if ((playerTotal==16 && (dealerUp>=9)) ||
        (playerTotal==15 && (dealerUp>=10)) ||
        (playerTotal==17 && dealerUp==11)) {
        cout << "Action: Surrender (-$" << (0.5 * bet) << ")" << endl;
        bankroll -= 0.5 * bet;
        return;
    }

    for (size_t i = 0; i < hands.size(); ++i) {
        auto& h = hands[i];
        bool completed = false;
        while (!completed) {
            int val = computeHandValue(h);
            if (val >= 21) break;

            std::string move = best_move_soft(h, dealerUp);
            if (move.empty()) move = best_move_hard(h, dealerUp);

            std::string dev = check_playing_deviations(h, dealerUp, shoe.getTrueCount());
            if (!dev.empty()) {
                if (dev == "Stand") move = "S";
                else if (dev == "Hit") move = "H";
                else if (dev == "Double") move = "D";
            }

            if (move == "S") {
                cout << "Hand " << (i+1) << ": Stand at " << computeHandValue(h) << (isSoft(h)?" (soft)":"") << endl;
                break;
            } else if (move == "D" || move == "Ds") {
                if (h.size() == 2) {
                    cout << "Hand " << (i+1) << ": Double" << endl;
                    h.push_back(drawCard());
                    handDoubled[i] = true;
                    completed = true;
                } else {
                    if (move == "Ds") {
                        cout << "Hand " << (i+1) << ": Stand at " << computeHandValue(h) << (isSoft(h)?" (soft)":"") << endl;
                        break;
                    } else {
                        h.push_back(drawCard());
                        cout << "Hand " << (i+1) << ": Hit -> total " << computeHandValue(h) << endl;
                    }
                }
            } else {
                h.push_back(drawCard());
                cout << "Hand " << (i+1) << ": Hit -> total " << computeHandValue(h) << endl;
            }
        }
    }

    while (computeHandValue(dealer) < 17) {
        dealer.push_back(drawCard());
    }

    auto showHand = [&](const std::vector<int>& h){
        for (size_t i=0;i<h.size();++i){
            cout << (i?", ":"[") << cardStr(h[i]);
        }
        cout << "] (" << computeHandValue(h) << ")";
    };

    cout << "Dealer final: "; showHand(dealer); cout << endl;

    int dealerTotal = computeHandValue(dealer);
    for (size_t i = 0; i < hands.size(); ++i) {
        auto& h = hands[i];
        int v = computeHandValue(h);
        int handBet = bet;
        if (handDoubled[i]) handBet *= 2;

        cout << "Resolve Hand " << (i+1) << ": "; showHand(h); cout << " vs Dealer " << dealerTotal;
        if (v > 21) {
            cout << " -> Lose (-$" << handBet << ")" << endl;
            bankroll -= handBet;
        } else if (dealerTotal > 21 || v > dealerTotal) {
            cout << " -> Win (+$" << handBet << ")" << endl;
            bankroll += handBet;
        } else if (dealerTotal > v) {
            cout << " -> Lose (-$" << handBet << ")" << endl;
            bankroll -= handBet;
        } else {
            cout << " -> Push" << endl;
        }
    }
    cout << "End of round: Bankroll $" << bankroll
         << ", Running Count " << shoe.getRunningCount()
         << ", True Count " << shoe.getTrueCount()
         << ", Cards Remaining " << shoe.getRemainingCards()
         << std::endl;
}
