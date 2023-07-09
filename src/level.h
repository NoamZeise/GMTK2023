#ifndef LEVEL_H
#define LEVEL_H

#include <vector>

#include "board.h"

const int HAND_SIZE = 5;

struct Level {
    std::vector<BoardPos> forest;
    std::vector<BoardPos> mountain;
    std::vector<BoardPos> lake;
    CounterType hand[HAND_SIZE];
};

std::vector<Level> loadLevels();

#endif
