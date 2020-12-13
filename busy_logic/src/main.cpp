#include "Arduino.h"
#include "bb_puzzle.h"
#include <Wire.h>

//#define S
#ifndef S
#pragma GCC poison Serial
#endif

// Set LED_BUILTIN if it is not defined by Arduino framework
#define LED_BUILTIN 17

class Gate {
public:
enum Mode {
  AND,
  OR,
  XOR,
  NOR,
};
protected:
  int b0,b1,led;
  Mode mode;
public:

  Gate(int _b0,int _b1,int _led,Mode m) : b0(_b0),b1(_b1),led(_led),mode(m){
  }
  void init() {
    pinMode(b0,INPUT);
    pinMode(b1,INPUT);
    pinMode(led,OUTPUT);
  }

  void update() {
    int v0=digitalRead(b0);
    int v1=digitalRead(b1);
    int val=calc(v0,v1);
    digitalWrite(led, val);
  }

  int calc(int v0,int v1) {
    switch(mode) {
      case AND:
        return (v0 && v1);
      case OR:
        return (v0 || v1);
      case XOR:
        return (v0 ^ v1);
      case NOR:
        return !(v0 || v1);
      default:
        return v0 && v1;
    }
  }

};


class XEvaluable {
public:
virtual uint8_t evaluate(XEvaluable &e...)=0;
};

class Input : XEvaluable {
  uint8_t pin;
public:
  Input(uint8_t _pin) : pin(_pin) {}
  void  init(){
    pinMode(pin,INPUT);
  }

  uint8_t evaluate() {
    return digitalRead(pin);
  }
};

class Output : XEvaluable {
  uint8_t pin;
public:
  Output(uint8_t _pin) : pin(_pin) {}
  void  init(){
    pinMode(pin,OUTPUT);
  }

  uint8_t evaluate() {
    uint8_t  val=1;
    digitalWrite(pin, val);
    return digitalRead(val);
  }
}
;




Gate g0(2,3,4, Gate::Mode::AND );
Gate g1(5,6,7, Gate::Mode::XOR );
Gate g2(8,9,10, Gate::Mode::XOR );
Gate g3(16,14,15, Gate::Mode::XOR );

int I1=18;
int I2=19;
int I3=6;
int I4=5;
int I5=8;
int I6=9;
int I7=16;
int I8=14;

int O1=4;
int O2=7;
int O3=10;
int O4=15;


// MISO
// MOSI
// SCK
// SS
// MSC_RESET
// 5v
// gnd

#define NSTATE 256
byte oState[NSTATE];


int bitCount(int u) {
     unsigned int uCount;

     uCount = u - ((u >> 1) & 033333333333) - ((u >> 2) & 011111111111);
     return ((uCount + (uCount >> 3)) & 030707070707) % 63;

}





// FIXME rename
void processI2C(int n);
void i2c_request();

void setup()
{
    pinMode(A2,OUTPUT);
#ifdef S
  Serial.begin(115200);
  Serial.println("startup...");
#endif

  Wire.begin(0x3b);
  Wire.onReceive(processI2C);
  Wire.onRequest(i2c_request);

  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  

  g0.init();
  g1.init();
  g2.init();
  g3.init();

  for(int i=0;i<NSTATE;i++ ) {
    oState[i]=(bitCount(i)%2)?0xf:0;
  }
}

void sb(int t){

    // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);

  // wait for a second
  delay(t);

  // turn the LED off by making the voltage LOW
  digitalWrite(LED_BUILTIN, LOW);

   // wait for a second
  delay(t);
}

enum State {
  INITIAL,
  INTRO,
  GAME,
//  WON,
};


long tNextReport=0;
State state=INITIAL;
bField introState;

bField intro(bField i) {
  if(introState!=i) {
    state=GAME;
  }
  long t=millis()/1000;

  bField o;
  o.b4= (t>>0)&1;
  o.b3= (t>>1)&1;
  o.b2= (t>>2)&1;
  o.b1= (t>>3)&1;
  long now=millis();
  if(now > tNextReport) {
    tNextReport=now+1000;

#ifdef S
    Serial.print("intro=");
    Serial.print(o.b1);
    Serial.print(o.b2);
    Serial.print(o.b3);
    Serial.print(o.b4);
    Serial.print(".");
    Serial.println();
#endif
  }
  return o;
}

bool solved=false;

bField lookupState(bField i) {
  int idx=i.toInt();
  int st=oState[idx];
  bField o;
  o.b4= (st>>0)&1;
  o.b3= (st>>1)&1;
  o.b2= (st>>2)&1;
  o.b1= (st>>3)&1;
  return o;
}

 int processed=0;
   static int readState = -1;


bField game(bField i) {
  static bField lastI;
//  bField o=bb_puzzle(i);
  bField o=lookupState(i);
  long now=millis();
  if(now > tNextReport || lastI != i) {
    lastI=i;
    tNextReport=now+1000;

    if(o.b1 && o.b2 && o.b3 && o.b4) {
      solved=true;
    }


#ifdef S
    Serial.print("state=");
    Serial.print(i.b1);
    Serial.print(i.b2);
    Serial.print(i.b3);
    Serial.print(i.b4);
    Serial.print(i.b5);
    Serial.print(i.b6);
    Serial.print(i.b7);
    Serial.print(i.b8);
    Serial.print(",");
    Serial.print(o.b1);
    Serial.print(o.b2);
    Serial.print(o.b3);
    Serial.print(o.b4);
    Serial.print(".");
      Serial.print(processed);
    Serial.print(";");
      Serial.print(readState);
      
    Serial.println();
#endif
  }
  return o;
}



bField processInputs(bField&i) {
  switch(state) {
    case INITIAL:
      introState=i;
      state=INTRO;
    case INTRO:
      return intro(i);
    case GAME:
      return game(i);
  }
  state=INTRO;
}


int xval(char c) {
  if('0'<=c && c<='9')
      return c-'0';
  if('a'<=c && c<='f')
    return c-'a'+10;
  return -1;
}


void processByte(char c) {

  processed++;
  if(readState==-1 && c=='R') {
    solved=false;
    state=INITIAL;
    return;
  }
  if(c=='!') {
    readState=0;
    return;
  }
  if(c=='@') {
    return;
  }
  if(readState<0) {
    return;
  }
  int val=xval(c);
  if(val < 0) {
    readState=-1;
    return;
  }

  if(readState<NSTATE) {
    oState[readState]=val;
    readState++;
  }
  if(readState>=NSTATE) {
    #ifdef S
      Serial.println("programmed!");
    #endif
    readState=-1;
  }
}

void processI2C(int n) {
  while(Wire.available()>0) {
    processByte(Wire.read());
  }
}

void i2c_request() {
  if(solved)
    Wire.write('S');
  else
    Wire.write('.');
}




#ifdef S
void processSerial() {
  while (Serial.available() > 0) {
    processByte(Serial.read());
  }
}
#endif
void loop() {
#ifdef S
  processSerial();
#endif
//  processI2C(1);

  bField i;
  i.b1=digitalRead(I1);
  i.b2=digitalRead(I2);
  i.b3=digitalRead(I3);
  i.b4=digitalRead(I4);
  i.b5=digitalRead(I5);
  i.b6=digitalRead(I6);
  i.b7=digitalRead(I7);
  i.b8=digitalRead(I8);

  bField o=processInputs(i);

  digitalWrite(O1,o.b1);
  digitalWrite(O2,o.b2);
  digitalWrite(O3,o.b3);
  digitalWrite(O4,o.b4);
  sb(1);

  static unsigned int k=0;
  k++;

  int bc=o.b1+o.b2+o.b3+o.b4;
  long t=millis()/1000;

  bool desired=((k%4)<bc);
  // if((t%37)==0)
  //   desired=1;
  // if((t%41)==0)
  //   desired=0;

  if(desired) 
    digitalWrite(A2,1);
  else 
   digitalWrite(A2,0);
  
}
