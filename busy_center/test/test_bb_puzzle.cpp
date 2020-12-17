#include <unity.h>
#include <stdio.h>
#include "bb_puzzle_gen.h"
//#include <Android.h>

void riddletest_begin() {

}

void bb_puzzle_check_one(int seed) {
  PuzzleSpec spec;
  spec.seed=seed;
  Puzzle p=  genPuzzle(spec);
  TEST_ASSERT(p.valid);
}

void bb_puzzle_check_all() {
  for(int i=0;i<1000;i++) {
     bb_puzzle_check_one(i);
  }
}

void bb_puzzle_check_one() {
  bb_puzzle_check_one(0);
}

void riddletest_end() {

}

void process() {
    UNITY_BEGIN();
    RUN_TEST(bb_puzzle_check_one);
    RUN_TEST(bb_puzzle_check_all);
    UNITY_END();
}
#ifdef ESP8266
#include <Arduino.h>

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!
}


void loop() {
  process();
}
#else
int main(int argc, char **argv) {
    process();
    return 0;
}

#endif
