#pragma once
#include <stdint.h>

#define PUZZLE_N        8
#define PUZZLE_OUTS     4
#define PUZZLE_MAX_TO_GENERATE 50
#define PUZZLE_N_STATES (1<<PUZZLE_N)

struct Puzzle {
    bool    valid;
    uint8_t state[PUZZLE_N_STATES];
};

struct PuzzleSpec {
    int seed;
};

void asdasd();
Puzzle genPuzzle(const PuzzleSpec&spec);
