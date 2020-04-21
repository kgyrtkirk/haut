#include <unity.h>
#include <stdio.h>
#include <bb_puzzle.h>

void riddletest_begin() {

}

void bb_puzzle_satifiable() {
  for(int x=0;x< (1<<8) ; x++) {
    bField i;
    i.b1=!!(x&(1<<0));
    i.b2=!!(x&(1<<1));
    i.b3=!!(x&(1<<2));
    i.b4=!!(x&(1<<3));
    i.b5=!!(x&(1<<4));
    i.b6=!!(x&(1<<5));
    i.b7=!!(x&(1<<6));
    i.b8=!!(x&(1<<7));
    bField o=bb_puzzle(i);
    if(o.b1 && o.b2 && o.b3 && o.b4) {
      return;
    }
  }
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