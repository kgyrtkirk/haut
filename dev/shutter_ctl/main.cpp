//#include <Arduino.h>
#include <wiringPi.h>
#include "rf433ctl.h"
#include <stdlib.h>
#include <stdio.h>


#define OUT 8
#define K	2

#define Serialprintln(X)	printf(X "\n")

class C {
public:
	RF433Ctl rfctl;

C() : rfctl(OUT)
{}


void sendDown() {
	Serialprintln("D");
	for(int i =0 ;i<K;i++){
		rfctl.cmd(i, -1);
	}
}

void sendUp() {
	Serialprintln("U");
	for(int i =0 ;i<K;i++){
		rfctl.cmd(i, 1);
	}
}

void sendStop(){
  Serialprintln("S");
	for(int i =0 ;i<K;i++){
		rfctl.cmd(i, 0);
	}
}


int main() {

//  Serial.begin(115200);
//  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
//  digital
  pinMode(OUT, OUTPUT);

  Serialprintln("x");
  delay(1000);
  sendDown();
  delay(20000);
  sendStop();
  delay(2000);
  sendUp();
  delay(3000);
  sendStop();


  // 16 down
  // 15.3 up
}
}
;
int main() {
  wiringPiSetup () ;
	{
	C	c;
	c.main();
	}
}



void loop() {
//	int	state=0;
//	digitalWrite(OUT, state);
//	for (int i = 0; i < sizeof(signal) / sizeof(int); i++) {
//		delayMicroseconds(signal[i]);
//		state = !state;
//		digitalWrite(OUT, state);
//	}


}

