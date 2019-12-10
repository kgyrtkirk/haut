#include <Arduino.h>

#include "rf433ctl.h"


#define OUT 6

RF433Ctl rfctl(OUT);

#define K	2

void sendDown() {
	Serial.println("D");
	for(int i =0 ;i<K;i++){
		rfctl.cmd(i, -1);
	}
}

void sendUp() {
	Serial.println("U");
	for(int i =0 ;i<K;i++){
		rfctl.cmd(i, 1);
	}
}

void sendStop(){
  Serial.println("S");
	for(int i =0 ;i<K;i++){
		rfctl.cmd(i, 0);
	}
}

void setup() {
  Serial.begin(115200);
//  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
//  digital
  pinMode(OUT, OUTPUT);

  Serial.println("x");
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



void loop() {
//	int	state=0;
//	digitalWrite(OUT, state);
//	for (int i = 0; i < sizeof(signal) / sizeof(int); i++) {
//		delayMicroseconds(signal[i]);
//		state = !state;
//		digitalWrite(OUT, state);
//	}


}

