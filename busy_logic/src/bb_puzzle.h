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
  bool operator==(const bField& o){ 
    return b1==o.b1 && b2==o.b2 && b3==o.b3 && b4==o.b4 &&
      b5==o.b5 && b6==o.b6 && b7==o.b7 && b8 == o.b8;
  }
  bool operator!=(const bField& o){ 
    return !(*this==o);
  }
  void negateAll(){ 
    b1=!b1;
    b2=!b2;
    b3=!b3;
    b4=!b4;
    b5=!b5;
    b6=!b6;
    b7=!b7;
    b8=!b8;
  }
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

// 05-06
  r.b1=(i2 && !i5 && !i8 && i3);
  r.b2=i7 ^ i2 ^ (i3);
  r.b3=i6^i7^ i4;
  r.b4=(i3 ^ i1 ) && i6;

// 05-07
  r.b1=(!i2 && i3 && i8 && !i4);
  r.b2=i2 ^ i5 ^ (i1);
  r.b3=i2^i7^ i4;
  r.b4=(i2 ^ i1 ) && i6;

// 05-08
  r.b1=(!i3 && i4 && i8 && !i6);
  r.b2=(i3 ^ i5 ) && i2;
  r.b3=i2^i7^ i4;
  r.b4=(i7 ^ i3 ) && i1;

// 05-09
  r.b1=(i1 && !i3 && !i5 && i8);
  r.b2=(i7 ^ !i6 ) && i2;
  r.b3=i2^i3^ i4;
  r.b4=(i7 ^ i5 ) && i2;

// 05-10
  r.b1=(i2 && !i4 && !i5 && i6);
  r.b2=(i1 ^ !i2 ) && i3;
  r.b3=i2^i5^ i7;
  r.b4=(i8 ^ i5 ) && i2;

// 05-11 (12)
  r.b1=(i2 ^ !i7 ) && i3;
  r.b2=i3^i5^ i2^ i4;
  r.b3=(i6 ^ i3 ) && i8 ;
  r.b4=(i1 && !i6 && !i5 && i7);

// 05-12 (13)
  r.b1=(i1 ^ !i5 ) && i3;
  r.b2=i7^i3^ i8 && i4 ;
  r.b3=(i8 ^ i2 )  ;
  r.b4=(i3 && !i5 && !i8 && i2);

// 05-13
  r.b1=(i2 ^ !i3 ) && i1;
  r.b2=(i7^i5^ i8) && i6 ;
  r.b3=(i8 ^ i2 )  ;
  r.b4=(i7 && !i5 && !i8 && i3);

// 05-14
// 05-15
  r.b1=(i1 ^ !i6 ) && i5;
  r.b2=(i2^i5^ i7) && i3 ;
  r.b3=(i3 ^ i8 )  ;
  r.b4=(i4 && !i1 && !i2 && i3);

// 05-17
  r.b1=(i5 ^ !i2 ) && i3;
  r.b2=(i7 && !i6 && !i8 && i5);
  r.b3=(i8 ^ i6 ^ i5 ^ i7 ^ i4)  ;
  r.b4=(i3^i4^ i5) && i1 ;

// 05-18
  r.b1=(i1 ^ !i5 ) && i7 && i5;
  r.b2=(i3 ^ i4 ^ i6 ^ i7 ^ i4)  ;
  r.b3=(i5 && !i4 && i7 && i3);
  r.b4=(i7^i8^ i2) && i2 ;

// 05-19
  r.b1=(i3 && !i1 && i2 && i5);
  r.b2=(i2 ^ !i6 ) && i5 && i3;
  r.b3=(i3 ^ i4 ^ i6 ^ !i7 ^ i4)  ;
  r.b4=(i1^i8^ i2) && i3 ;

// 05-19
  r.b1=(i4 && i1 && !i2 && i3);
  r.b2=(i7 ^ i6 ) && i1 ;
  r.b3=(i8 ^ i7 ^ i6 ^ !i1 ^ i4)  ;
  r.b4=(i1^i4^ i3) && i5 ;

// 05-22
  r.b1=(i1 && i8 && !i2 && i7);
  r.b2=(i4 ^ i6 ) && i3 ;
  r.b3=(i5 ^ i4 ^ i8 ^ !i2 ^ i7)  ;
  r.b4=(i4^i2^ i3) && i7 ;

// 05-26
  r.b1=(i1 && !i8 && !i2 && i3);
  r.b2=(i2 ) ^ ( i7 && i6 );
  r.b3=(i1 ^ !i4 ^ !i6 ^ i8)  ;
  r.b4=(i2^i3^ i5) && i7 ;

// 05-27
  i.negateAll();
  r.b1=(i1 && !i8 && !i2 && i3);
  r.b2=(i2 ) ^ ( i7 && i6 );
  r.b3=(i1 ^ !i4 ^ !i6 ^ i8)  ;
  r.b4=(i2^i3^ i5) && i7 ;

// 05-28 - unsolved
  i.negateAll();
  r.b1=(i4 && i1 && !i2 && i3);
  r.b2=(i7 ^ i6 ) && i1 ;
  r.b3=(i8 ^ i7 ^ i6 ^ !i1 ^ i4)  ;
  r.b4=(i1^i4^ i3) && i5 ;

// there was some issue with saving/uploading
// not sure if previous one was even loaded

// 06-03
  r.b1=(i1 ^ !i3 ) && i1;
  r.b2=(i7^i6^ i8) && i2 ;
  r.b3=(i8 ^ i5 )  ;
  r.b4=(i7 && !i6 && !i8 && i3);

// 06-18
  r.b1=(!i1 ^ !i5 ) && i2;
  r.b2=(!i2^!i6^ i7) && i8 ;
  r.b3=(!i3 ^ !i7 )  ;
  r.b4=(!i4 && i8 && !i3 && i6);



if(false) {
  r.b1=!r.b1;
  r.b2=!r.b2;
  r.b3=!r.b3;
  r.b4=!r.b4;
}

  return r;
}
