#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;

enum Result { PLAYER_WIN, DEALER_WIN, DRAW };

const int DECKS_IN_SHOE = 8;
const int CARDS_PER_DECK = 52;
const double RESHUFFLE_THRESHOLD = 0.2;

class Shoe {
public:
        Shoe(mt19937& rng) : rng(rng) {
        refill();
    }

    void refill() {
        deck.clear();
        for (int d = 0; d < DECKS_IN_SHOE; ++d) {
            for (int i = 1; i <= 13; ++i) {
                for (int j = 0; j < 4; ++j) {
                    deck.push_back((i > 10) ? 10 : i);
                }
            }
        }
        shuffle(deck.begin(), deck.end(), rng);
        nextCardIndex = 0;
    }

    int draw() {
        if (remainingCards() <= RESHUFFLE_THRESHOLD * totalSize()) {
            refill();
        }
        return deck[nextCardIndex++];
    }


private:
    mt19937& rng;
    vector<int> deck;
    size_t nextCardIndex = 0;

    size_t remainingCards() const {
        return deck.size() - nextCardIndex;
    }

    size_t totalSize() const {
        return DECKS_IN_SHOE * CARDS_PER_DECK;
    }
};


    int computeHandValue(const vector<int>& hand) {
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

// Given a pair + upcard, split or no split
bool shouldSplit(int pairValue, int dealerUpcard) {

    if (dealerUpcard == 1) dealerUpcard = 11;

    switch (pairValue) {
        case 1: // Aces
            return true;
        case 2:
        case 3:
            return dealerUpcard >= 2 && dealerUpcard <= 7;
        case 4:
            return dealerUpcard == 5 || dealerUpcard == 6;
        // who splits 5s??
        case 5:
            return false;
        case 6:
            return dealerUpcard >= 2 && dealerUpcard <= 6;
        case 7:
            return dealerUpcard <= 7;
        case 8:
            return true; // Always split 8s
        case 9:
            return (dealerUpcard <= 6 || dealerUpcard == 8 || dealerUpcard == 9);
        case 10:
            return false;
        default:
            return false;
    }
}

bool shouldDoubleDown(const vector<int>& hand, int dealerUpcard) {
    if (dealerUpcard == 1) dealerUpcard = 11;

    int total = 0, aces = 0;
    for (int card : hand) {
        total += card;
        if (card == 1) aces++;
    }

    bool isSoft = (aces > 0 && total + 10 <= 21);
    int value = total;
    if (isSoft) value += 10;

    if (isSoft) {
        switch (value) {
            case 13:
            case 14:
                return dealerUpcard == 5 || dealerUpcard == 6;
            case 15:
            case 16:
                return dealerUpcard >= 4 && dealerUpcard <= 6;
            case 17:
                return dealerUpcard >= 3 && dealerUpcard <= 6;
            case 18:
                return dealerUpcard >= 2 && dealerUpcard <= 6;
            case 19:
                return dealerUpcard == 6;
            default:
                return false;
        }
    } else {
        switch (value) {
            case 9:
                return dealerUpcard >= 3 && dealerUpcard <= 6;
            case 10:
                return dealerUpcard >= 2 && dealerUpcard <= 9;
            case 11:
                return true;
            default:
                return false;
        }
    }
}

// Simulate one game of blackjack
Result playGame(Shoe& shoe, int& surrenderCounter) {
    auto draw = [&]() {
        return shoe.draw();
    };

    vector<int> player = { draw(), draw() };
    vector<int> dealer = { draw(), draw() };  // dealer[0] is visible

    int dealerUpcard = dealer[0] == 1 ? 11 : dealer[0]; // treat ace as 11 for lookup

    auto isSoft = [](const vector<int>& hand) {
        int total = 0, aces = 0;
        for (int card : hand) {
            total += card;
            if (card == 1) aces++;
        }
        return (aces > 0 && total + 10 <= 21);
    };

    auto handValue = [](const vector<int>& hand) {
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
    };

    int playerTotal = handValue(player);

    // Check for player blackjack
    if (player.size() == 2 && playerTotal == 21) {
        return PLAYER_WIN;
    }


    // Late surrender strategy
    if ((playerTotal == 16 && (dealerUpcard == 9 || dealerUpcard == 10 || dealerUpcard == 11)) ||
        (playerTotal == 15 && (dealerUpcard == 10 || dealerUpcard == 11)) ||
        (playerTotal == 17 && dealerUpcard == 11)) {
        surrenderCounter++;
        return DEALER_WIN;  // surrender counts as a half-loss (still tracked under dealerWins)
    }

    // Player turn
    while (true) {
        playerTotal = handValue(player);
        bool soft = isSoft(player);

        if (!soft) { // hard totals
            if (playerTotal >= 17) break;
            if (playerTotal >= 13 && playerTotal <= 16 && dealerUpcard <= 6) break;
            if (playerTotal == 12 && dealerUpcard >= 4 && dealerUpcard <= 6) break;
            if (playerTotal <= 11) player.push_back(draw());
            else player.push_back(draw());
        } else { // soft totals
            if (playerTotal >= 19) break;
            if (playerTotal == 18 && dealerUpcard <= 8) break;
            player.push_back(draw());
        }
    }

    playerTotal = handValue(player);
    if (playerTotal > 21) return DEALER_WIN;

    // Dealer turn
    while (handValue(dealer) < 17) {
        dealer.push_back(draw());
    }

    int dealerTotal = handValue(dealer);
    if (dealerTotal > 21) return PLAYER_WIN;
    if (playerTotal > dealerTotal) return PLAYER_WIN;
    if (dealerTotal > playerTotal) return DEALER_WIN;
    return DRAW;
}

//
void simulateGames(int gamesPerThread, int& playerWins, int& dealerWins, int& draws, int& surrenders) {

    mt19937 rng(chrono::system_clock::now().time_since_epoch().count() + std::hash<thread::id>{}(this_thread::get_id()));

    int localPlayerWins = 0, localDealerWins = 0, localDraws = 0, localSurrenders = 0;

    // Shoe per thread
    Shoe shoe(rng);

    for (int i = 0; i < gamesPerThread; ++i) {
        Result result = playGame(shoe, localSurrenders);
        if (result == PLAYER_WIN) localPlayerWins++;
        else if (result == DEALER_WIN) localDealerWins++;
        else localDraws++;
    }

    playerWins = localPlayerWins;
    dealerWins = localDealerWins;
    draws = localDraws;
    surrenders = localSurrenders;
}

int main() {
    // 100M (1-10M should be fine)
    const int totalGames = 1000000;

    //half cores
    const int numThreads = (thread::hardware_concurrency()) / 2;
    const int gamesPerThread = totalGames / numThreads;

    // vectors
    vector<thread> threads;
    vector<int> playerWins(numThreads);
    vector<int> dealerWins(numThreads);
    vector<int> draws(numThreads);
    vector<int> surrenders(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(simulateGames, gamesPerThread, ref(playerWins[i]), ref(dealerWins[i]), ref(draws[i]),  ref(surrenders[i]));
    }

    // wait for all threads to be done
    for (auto& t : threads) {
        t.join();
    }

    // Merge results
    int totalPlayerWins = 0, totalDealerWins = 0, totalDraws = 0, totalSurrenders = 0;
    for (int i = 0; i < numThreads; ++i) {
        totalPlayerWins += playerWins[i];
        totalDealerWins += dealerWins[i];
        totalDraws += draws[i];
        totalSurrenders += surrenders[i];
    }

    cout << "Total games: " << totalGames << endl;
    cout << "Player wins: " << totalPlayerWins << endl;
    cout << "Dealer wins: " << totalDealerWins << endl;
    cout << "Draws:       " << totalDraws << endl;
    cout << "Surrenders:   " << totalSurrenders << endl;

    return 0;
}
