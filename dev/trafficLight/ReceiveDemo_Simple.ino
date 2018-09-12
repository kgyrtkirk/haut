#include "Arduino.h"
#include <avr/sleep.h>
#define	RED_PIN			0
#define	GREEN_PIN		3
#define	YELLOW_PIN		1

void setup() {
	pinMode(RED_PIN, OUTPUT);
	pinMode(YELLOW_PIN, OUTPUT);
	pinMode(GREEN_PIN, OUTPUT);
}

void test() {
	for (int i = 0; i < 10; i++) {
		digitalWrite(RED_PIN, 1);
		digitalWrite(YELLOW_PIN, 1);
		digitalWrite(GREEN_PIN, 1);
		delay(100);
		digitalWrite(RED_PIN, 0);
		digitalWrite(YELLOW_PIN, 0);
		digitalWrite(GREEN_PIN, 0);
		delay(100);
	}

}

void lights(int t,int r,int y,int g){
	digitalWrite(RED_PIN, r);
	digitalWrite(YELLOW_PIN, y);
	digitalWrite(GREEN_PIN, g);
	delay(t);
	digitalWrite(RED_PIN, 0);
	digitalWrite(YELLOW_PIN, 0);
	digitalWrite(GREEN_PIN, 0);
}
void loop() {
	lights(10000,1,0,0);
	lights(1000,1,1,0);
	lights(5000,0,0,1);
	lights(2000,0,1,0);

}
