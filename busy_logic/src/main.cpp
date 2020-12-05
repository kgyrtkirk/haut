#include "Arduino.h"
#include "bb_puzzle.h"

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

int I1=2;
int I2=3;
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


void setup()
{
  Serial.begin(115200);
  Serial.println("startup...");

  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  


  g0.init();
  g1.init();
  g2.init();
  g3.init();
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
  o.b1= (t>>0)&1;
  o.b2= (t>>1)&1;
  o.b3= (t>>2)&1;
  o.b4= (t>>3)&1;
  long now=millis();
  if(now > tNextReport) {
    tNextReport=now+1000;

    Serial.print("intro=");
    Serial.print(o.b1);
    Serial.print(o.b2);
    Serial.print(o.b3);
    Serial.print(o.b4);
    Serial.print(".");
    Serial.println();
  }
  return o;
}

bField game(bField i) {
  bField o=bb_puzzle(i);
  long now=millis();
  if(now > tNextReport) {
    tNextReport=now+1000;

    Serial.print("state=");
    Serial.print(o.b1);
    Serial.print(o.b2);
    Serial.print(o.b3);
    Serial.print(o.b4);
    Serial.print(".");
    Serial.println();
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

void loop() {
//  g0.update();
//  g1.update();
//  g2.update();
//  g3.update();

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
  sb(10);
}
