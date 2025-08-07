# Advantage-Play Blackjack Simulator

C++17 blackjack simulator with Hi-Lo counting, basic strategy + selected playing deviations, configurable bet spread, and Monte Carlo EV/Risk-of-Ruin.

## Build

Main simulator (EV and Risk of Ruin):

```
g++ -std=c++17 -Iinclude src/Game.cpp src/Shoe.cpp src/HandEval.cpp src/Strategy.cpp src/Simulation.cpp main.cpp -o blackjack_sim -pthread
```

Trace tool (verbose per-round actions):

```
g++ -std=c++17 -Iinclude src/Game.cpp src/Shoe.cpp src/HandEval.cpp src/Strategy.cpp src/Simulation.cpp src/TraceDemo.cpp -o trace_demo -pthread
```

## Run

Monte Carlo sessions (prints EV and Risk of Ruin):

```
./blackjack_sim
```

Verbose trace for N rounds (see all moves, counts, and cards remaining):

```
./trace_demo 10
```

## Configuration

Edit `main.cpp` to control:
- Games per session: `gamesPerSession`
- Number of sessions (Monte Carlo trials): `numSessions`
- Starting bankroll: `startingBankroll`
- Bet unit (e.g., $10): `betUnit`
- Bet sizing mapping by True Count: the `betSizing` lambda. Returning 0 means sit out (no wager, cards still consumed).

Edit `src/TraceDemo.cpp` to keep trace output consistent with your bet mapping.

Shoe behavior:
- 8-deck shoe by default (change in `include/Shoe.h`)
- Reshuffles between rounds only (cut-card at ~80% penetration)
- Hi-Lo running count maintained internally; True Count computed using decks remaining

# Only read below if you actually care about Advantage Play in Blackjack...
## Strategy guide for advantage players

Memorize basic strategy:
- Always know the correct hit/stand/double/split for hard totals, soft totals, and pairs versus each dealer upcard.

Chart for H17 (Dealer will hit on a Soft 17) Ruleset:
https://www.blackjackapprenticeship.com/wp-content/uploads/2024/09/H17-Basic-Strategy.pdf

Chart for S17 (Dealer will NOT hit on a Soft 17) Ruleset:
https://www.blackjackapprenticeship.com/wp-content/uploads/2024/09/S17-Basic-Strategy.pdf

Hi-Lo card counting (used here):
- Card values: 2–6 = +1, 7–9 = 0, 10s and Aces = −1
- Running Count (RC): sum of the above as cards are exposed
- Decks remaining: cards left in shoe ÷ 52
- True Count (TC): RC ÷ decks remaining (rounded/truncated, don't be spending too much time tryna do division)

Betting by True Count (example mapping in `main.cpp`):
- TC ≤ 0: 0 units (sit out)
- TC 1: 1 unit
- TC 2: 2 units
- TC 3: 4 units
- TC 4: 6 units
- TC ≥ 5: 8 units

Playing deviations:
- Deviations are intentional departures from basic strategy driven by the True Count (e.g., standing on 16 vs 10 at TC ≥ 0, doubling 10 vs 10 at TC ≥ 4).

Trace output explains per-round actions, bankroll changes, running/true count, and cards remaining. In sit-out rounds (units = 0), the game still consumes cards without changing bankroll.

I've also made a Blackjack Trainer that forces optimal moves to help practice basic strategy, deviations and counting (if you're interested):

https://github.com/VanBaNguyen/AP-Blackjack-Trainer

## Favourable Rules

Most Casinos I have seen usually run 3:2 Blackjack, H17, and doubling after spliting (DAS). Generally speaking, the other rules don't matter as much but still affect house edge.

That said, if you see a Blackjack table running 6:5 Blackjack, please for the love of god, do not sit down and play. It's mathematically impossible to gain an edge over the long term, even under extremely ideal circumstances.

S17 is really good for players but watch out for other things like deck count & penetration.

Number of decks: How many decks are used in the game (e.g., 6 or 8). More decks = tougher for players.

Deck penetration: The % of cards dealt before reshuffling. Higher penetration = more cards seen before shuffle.

You want high Deck Pen & Lower # of decks, but that sort of thing is out of your control most of the time.

## Side Note 1

I'm not pulling these strats out of nowhere, these are very well known methods, you're welcome to read any of Stanford Wong's books to learn more (even though some of the stuff is a bit outdated).

## Side Note 2

Even if your strategy & counting are perfect under ideal rulesets, there's still a chance you don't make money or possibly worse, lose everything. Which is why APs usually use software (similar to this program) to find betspreads with extremely low risk of ruin, but there's always a chance. Don't be that person who runs a 40% ROR strategy and promptly wonders why they lost so much money.