#include <avr/wdt.h>

//#define DIAG
#ifdef DIAG

#include "Arduino.h"

void setup() {
  // initialize digital pin 13 as an output.
  pinMode(4, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(4, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(4, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second
}


#else

#include "Arduino.h"
//The setup function is called once at startup of the sketch
#include "TinyWireS.h"



// The loop function is called in an endless loop

int state=0;

// speed of up/down in 100s of milisecs
uint8_t		speed_up=1;
uint8_t		speed_down=1;

uint8_t 	target_val=128;
uint8_t 	orig_val=0;
uint8_t 	val=0;
unsigned long int cmd_time;

#define LED_PIN 1
void setup() {
	MCUSR &= ~(1<<WDRF); // reset status flag
	wdt_disable();
	wdt_reset();
	wdt_enable(WDTO_2S);
	pinMode(LED_PIN, OUTPUT);
	state=1;
	digitalWrite(LED_PIN, state);
	TinyWireS.begin(0x33); // initialize I2C lib & setup slave's address (7 bit - same as Wire)
	analogWrite(4,128);
/*	speed_up=speed_down=1;
	orig_val=val=0;
	target_val=128;
	cmd_time=millis();*/
}

void loop() {
	if (TinyWireS.available()) {
		uint8_t new_target_val=TinyWireS.receive();
		speed_up=TinyWireS.receive();
		speed_down=TinyWireS.receive();
		wdt_reset();
		state=!state;
		digitalWrite(LED_PIN, state);
		if(new_target_val != target_val) {
			target_val=new_target_val;
			cmd_time=millis();
			orig_val=val;
		}
	}
	if (val != target_val) {
		uint16_t dt = (millis() - cmd_time);
		if (val < target_val) {
			uint8_t deltaVal = dt / speed_up * 255 / 100;
			if (deltaVal >= target_val - orig_val) {
				val = target_val;
			} else {
				val = orig_val + deltaVal;
			}
		} else {
			uint8_t deltaVal = dt / speed_down * 255 / 100;
			if (deltaVal >= orig_val - target_val) {
				val = target_val;
			} else {
				val = orig_val - deltaVal;
			}
		}
		analogWrite(4, val);
	}

}

#endif
