#include <unity.h>
#include <stdio.h>

void riddletest_begin() {

}

void riddletest_putchar(char c) {
  putchar(c);
}

void riddletest_flush() {
  fflush(stdout);
}

void riddletest_end() {

}

void process() {
    UNITY_BEGIN();
    RUN_TEST(riddletest_flush);
    UNITY_END();
}

int main(int argc, char **argv) {
    process();
    return 0;
}