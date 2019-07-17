// Do not remove the include below
#include "esp_gen3.h"


// Pin config
int LED_SONOFF = 2;

KMqttClient	kmqtt;


void setup() {
	Serial.begin(115200);
	kmqtt.init("dev");

	// initialise digital pin LED_SONOFF as an output.
	pinMode(LED_SONOFF, OUTPUT);

}

void b(int BLINK_DURATION) {
	digitalWrite(LED_SONOFF, LOW); // LOW will turn on the LED
	delay(BLINK_DURATION);
	digitalWrite(LED_SONOFF, HIGH); // HIGH will turn off the LED
	delay(BLINK_DURATION);

}

void bl() {
	int k=1000;
	int j=50;
	b(j);k-=j+j;
	b(j);k-=j+j;
	b(j);k-=j+j;
	delay(k);
}


void loop() {
	kmqtt.loop();
	bl();
}
