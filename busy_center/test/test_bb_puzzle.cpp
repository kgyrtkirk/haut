#include <unity.h>
#include <stdio.h>
#include "bb_puzzle_gen.h"
//#include <Android.h>

void riddletest_begin() {

}

void bb_puzzle_satifiable() {
  PuzzleSpec spec;
  spec.seed=100;
  //Serial.begin();
  Puzzle p=  genPuzzle(spec);
  printf("asd\n");
  TEST_FAIL_MESSAGE("not satisifable!");
}

void riddletest_end() {

}

void process() {
    UNITY_BEGIN();
    RUN_TEST(bb_puzzle_satifiable);
    UNITY_END();
}

int main(int argc, char **argv) {
    process();
    return 0;
}