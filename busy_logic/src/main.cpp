#include "Arduino.h"

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

Gate g0(2,3,4, Gate::Mode::AND );
Gate g1(5,6,7, Gate::Mode::XOR );
Gate g2(8,9,10, Gate::Mode::XOR );
Gate g3(16,14,15, Gate::Mode::XOR );

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

void loop() {
  g0.update();
  g1.update();
  g2.update();
  g3.update();
  sb(100);
}

