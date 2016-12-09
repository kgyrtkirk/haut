#include "Arduino.h"
//The setup function is called once at startup of the sketch
#include "TinyWireS.h"

#define LED_PIN 1
void setup() {

	pinMode(LED_PIN, OUTPUT);
	TinyWireS.begin(0x33); // initialize I2C lib & setup slave's address (7 bit - same as Wire)
	analogWrite(4,0);
}

// The loop function is called in an endless loop

int state=0;

void loop() {
	if(TinyWireS.available()){
		uint8_t val=TinyWireS.receive();
		analogWrite(4,val);
		state=!state;
		digitalWrite(LED_PIN, state);
	}
//	delay(100);
//	digitalWrite(LED_PIN, 0);
//	delay(100);
//	digitalWrite(LED_PIN, 1);
//	delay(100);
//	digitalWrite(LED_PIN, 0);
//	delay(1000);
}
