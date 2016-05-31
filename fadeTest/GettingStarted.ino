#include "Arduino.h"
#include "KRF.h"

#include "wiring_private.h"

int ledPin = 9;    // LED connected to digital pin 9

KRF krf(7, 8, KRF_KITCHEN_STRIP);

int my_putc( char c, FILE *t) {
  Serial.write( c );
}
//#include "bootloaders/optiboot/optiboot.h"

void setup() {
	Serial.begin(115200);
	Serial.println(KRF_KITCHEN_STRIP);
  fdevopen( &my_putc, 0);
	krf.begin();
	krf.listenTo(1,KRF_DESK0);
//	cbi(TCCR1B, CS11);
//	cbi(TCCR1B, CS10);
}
//#include "avr/boot.h"
void loop() {
//	krf.debug();
	krf.listen(20000);
//	Serial.println(krf.state.hum)
	analogWrite(ledPin, krf.state.strip_bright);

//	// fade in from min to max in increments of 5 points:
//	for (int fadeValue = 0; fadeValue <= 255; fadeValue += 5) {
//		// sets the value (range from 0 to 255):
//		analogWrite(ledPin, fadeValue);
//		// wait for 30 milliseconds to see the dimming effect
//		delay(30);
//	}
//	digitalWrite(13, 0);
//	// fade out from max to min in increments of 5 points:
//	for (int fadeValue = 255; fadeValue >= 0; fadeValue -= 5) {
//		// sets the value (range from 0 to 255):
//		analogWrite(ledPin, fadeValue);
//		// wait for 30 milliseconds to see the dimming effect
//		delay(30);
//	}
}
