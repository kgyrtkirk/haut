/*
 Blink LED - Sonoff Basic
 */

#include "Arduino.h"
// Pin config
int LED_SONOFF = 13;

void setup() {
	// initialise digital pin LED_SONOFF as an output.
	pinMode(LED_SONOFF, OUTPUT);
}

void b(int BLINK_DURATION) {
	digitalWrite(LED_SONOFF, LOW); // LOW will turn on the LED
	delay(BLINK_DURATION);
	digitalWrite(LED_SONOFF, HIGH); // HIGH will turn off the LED
	delay(BLINK_DURATION);

}

void loop() {
	b(100);
	b(100);
	delay(600);
}
