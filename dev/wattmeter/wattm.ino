#include "Arduino.h"

#define VT_PIN A1
#define AT_PIN A0

void setup() {
	Serial.begin(9600);
}

/**
 * 	returns	vcc estimation
 */
float readVCC() {
	long result; // Read 1.1V reference against AVcc
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	delay(2); // Wait for Vref to settle
	ADCSRA |= _BV(ADSC); // Convert
	while (bit_is_set(ADCSRA,ADSC));
	result = ADCL;
	result |= ADCH<<8;
//	result = 1125300L / result; // Back-calculate AVcc in mV 1.1*1023*1000
	return 1.1*1023/result;
}


void loop() {
	int vt_read = analogRead(VT_PIN);
	int at_read = analogRead(AT_PIN);
	float	vcc = readVCC();

	float voltage = vt_read * (vcc / 1023.0) * 5.0;
	float current = at_read * (vcc / 1023.0);
//	voltage=voltage*vccK/5000;
//	current=current*vccK/5000;

	float watts = voltage * current;

	Serial.print("VCC: ");
	Serial.print(vcc);
	Serial.print("\tVolts: ");
	Serial.print(voltage, 3);
	Serial.print("\tAmps: ");
	Serial.print(current, 3);
	Serial.print("\tWatts: ");
	Serial.println(watts, 3);
	Serial.println();

	delay(500);
}

