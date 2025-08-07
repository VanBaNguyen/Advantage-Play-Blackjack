#pragma once

enum Result { PLAYER_WIN, DEALER_WIN, DRAW };

static constexpr int DECKS_IN_SHOE       = 8;
static constexpr int CARDS_PER_DECK      = 52;
static constexpr double RESHUFFLE_THRESHOLD = 0.2;
