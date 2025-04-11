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


int drawCard(mt19937& rng) {
    uniform_int_distribution<int> dist(1, 13);
    int card = dist(rng);
    return (card > 10) ? 10 : card;
}

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

// Simulate one game of blackjack
Result playGame(mt19937& rng) {
    vector<int> player = { drawCard(rng), drawCard(rng) };
    vector<int> dealer = { drawCard(rng), drawCard(rng) };

    while (computeHandValue(player) < 17) {
        player.push_back(drawCard(rng));
    }
    int playerTotal = computeHandValue(player);
    if (playerTotal > 21) return DEALER_WIN;

    while (computeHandValue(dealer) < 17) {
        dealer.push_back(drawCard(rng));
    }
    int dealerTotal = computeHandValue(dealer);
    if (dealerTotal > 21) return PLAYER_WIN;

    if (playerTotal > dealerTotal) return PLAYER_WIN;
    if (dealerTotal > playerTotal) return DEALER_WIN;
    return DRAW;
}


void simulateGames(int gamesPerThread, int& playerWins, int& dealerWins, int& draws) {

    mt19937 rng(chrono::system_clock::now().time_since_epoch().count() + std::hash<thread::id>{}(this_thread::get_id()));

    int localPlayerWins = 0, localDealerWins = 0, localDraws = 0;

    for (int i = 0; i < gamesPerThread; ++i) {
        Result result = playGame(rng);
        if (result == PLAYER_WIN) localPlayerWins++;
        else if (result == DEALER_WIN) localDealerWins++;
        else localDraws++;
    }

    playerWins = localPlayerWins;
    dealerWins = localDealerWins;
    draws = localDraws;
}

int main() {
    // 100M (1-10M should be fine)
    const int totalGames = 10000000;

    //all cores
    const int numThreads = thread::hardware_concurrency();
    const int gamesPerThread = totalGames / numThreads;

    vector<thread> threads;
    vector<int> playerWins(numThreads);
    vector<int> dealerWins(numThreads);
    vector<int> draws(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(simulateGames, gamesPerThread, ref(playerWins[i]), ref(dealerWins[i]), ref(draws[i]));
    }

    for (auto& t : threads) {
        t.join();
    }

    // Merge results
    int totalPlayerWins = 0, totalDealerWins = 0, totalDraws = 0;
    for (int i = 0; i < numThreads; ++i) {
        totalPlayerWins += playerWins[i];
        totalDealerWins += dealerWins[i];
        totalDraws += draws[i];
    }

    cout << "Total games: " << totalGames << endl;
    cout << "Player wins: " << totalPlayerWins << endl;
    cout << "Dealer wins: " << totalDealerWins << endl;
    cout << "Draws:       " << totalDraws << endl;

    return 0;
}
