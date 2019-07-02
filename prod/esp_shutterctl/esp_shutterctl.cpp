#include "esp_shutterctl.h"
#include "rf433ctl.h"
#include "Arduino.h"

// Pin config
int LED_SONOFF = 2;
int LED_SONOFF2 = 16;

RF433Ctl rfctl(1);

void setup() {
	// initialise digital pin LED_SONOFF as an output.
	pinMode(LED_SONOFF, OUTPUT);

	rfctl.init();

}

void b(int BLINK_DURATION) {
	digitalWrite(LED_SONOFF, LOW); // LOW will turn on the LED
	delay(BLINK_DURATION);
	digitalWrite(LED_SONOFF, HIGH); // HIGH will turn off the LED
	delay(BLINK_DURATION);

}

void loop() {
	rfctl.cmd(2, -1);
	b(100);
	b(100);
	b(100);
	delay(600);
}
