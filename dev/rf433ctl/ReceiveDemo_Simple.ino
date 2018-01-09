#include <Arduino.h>

#include "rf433ctl.h"


#define OUT 6

RF433Ctl rfctl(OUT);

void sendDown() {
	  Serial.println("D");
		rfctl.cmd(0, -1);
		rfctl.cmd(1, -1);
}

void sendUp() {
	rfctl.cmd(0, 1);
}

void sendStop(){
	  Serial.println("S");
	rfctl.cmd(0, 0);
	rfctl.cmd(1, 0);
}

void setup() {
  Serial.begin(115200);
//  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
//  digital
  pinMode(OUT, OUTPUT);

  Serial.println("x");
  delay(1000);
  sendDown();
  delay(2000);
  sendStop();
  delay(2000);
  sendUp();
  sendUp();
  sendUp();
  delay(2000);
  sendStop();


  // 16 down
  // 15.3 up
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

