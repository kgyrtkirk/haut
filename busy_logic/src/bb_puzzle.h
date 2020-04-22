#include "stdint.h"

struct bField {
  uint32_t  b1:1;
  uint32_t  b2:1;
  uint32_t  b3:1;
  uint32_t  b4:1;
  uint32_t  b5:1;
  uint32_t  b6:1;
  uint32_t  b7:1;
  uint32_t  b8:1;
};



bField bb_puzzle(bField i) {
  bField r;
#define i1 i.b1
#define i2 i.b2
#define i3 i.b3
#define i4 i.b4
#define i5 i.b5
#define i6 i.b6
#define i7 i.b7
#define i8 i.b8

  r.b1=i1&&i2 && i7 && i8;
  r.b2=i3 ^ i4;
  r.b3=i5 && i4 ;
  r.b4=i5 ^ i6;

  return r;
}
