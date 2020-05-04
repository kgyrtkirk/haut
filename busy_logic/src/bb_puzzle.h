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

// non-solved 04-29
  r.b2=i1 ^ ( i2 && i8 && i4);
  r.b4=i1 ^ i3 ^ ( !i2 ^ i6 ^ i7 ^i5 );
  r.b1=i1 ^ i2 ^ i7;
  r.b3=!i2 && i3;
// 04-30
  r.b1=i3 ^ ( i2 ^ i8 ^ !i4);
  r.b2=i1 ^ i3 ^ ( !i2 ^ i6 ^ i7 ^i5 );
  r.b3=i1 ^ i2 ^ i7;
  r.b4=!i2 && i3;

// 05-01
  r.b1=i3 && i7;
  r.b2=(!i5 || !i8) && i6;
  r.b3=i1 ^ !i3;
  r.b4=i2 ^ i4 ^ i7 ^ i8;

// 05-02
  r.b1=!(i1 && i5) && i2;
  r.b2=!(i3 && i7) && i8;
  r.b3=i1 ^ i2 ^ i3;
  r.b4=i6^i7^i8 ^ i4;

// 05-05
  r.b1=(i2 && !i5 && !i8);
  r.b2=(i1 ^ i2 ) && i7;
  r.b3=i8 ^ i2 ^ i3;
  r.b4=i6^i7^i8 ^ i4;

if(false) {
  r.b1=!r.b1;
  r.b2=!r.b2;
  r.b3=!r.b3;
  r.b4=!r.b4;
}

  return r;
}
