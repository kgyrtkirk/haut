#include "sonar.h"
#include "Arduino.h"
#include "Adafruit_VL53L0X.h"

/*
 HC-SR04 Ping distance sensor]
 VCC to arduino 5v GND to arduino GND
 Echo to Arduino pin 13 Trig to Arduino pin 12
 Red POS to Arduino pin 11
 Green POS to Arduino pin 10
 560 ohm resistor to both LED NEG and GRD power rail
 More info at: http://goo.gl/kJ8Gl
 Original code improvements to the Ping sketch sourced from Trollmaker.com
 Some code and wiring inspired by http://en.wikiversity.org/wiki/User:Dstaub/robotcar
 */

#define trigPin 52
#define echoPin 53

UHSonar::UHSonar(int a) {
}
void UHSonar::init() {
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
}

#define	MAX_DIST	500

/**
 * measure/return distance in 'cm'
 */
long UHSonar::measure() {

	long duration, distance;
	digitalWrite(trigPin, LOW);  // Added this line
	delayMicroseconds(2); // Added this line
	digitalWrite(trigPin, HIGH);
//  delayMicroseconds(1000); //- Removed this line
	delayMicroseconds(12); // Added this line
	digitalWrite(trigPin, LOW);
	duration = pulseIn(echoPin, HIGH, 20000);
	if (duration == 0)
		return MAX_DIST;
	distance = (duration / 2) / 29.1;

	return distance;
}

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

LoxSonar::LoxSonar(int a) {
}
void LoxSonar::init() {
	if (!lox.begin()) {
		Serial.println(F("Failed to boot VL53L0X"));
		while (1)
			;
	}

}
long LoxSonar::measure() {
	VL53L0X_RangingMeasurementData_t measure;
	lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

	if (measure.RangeStatus != 4) {  // phase failures have incorrect data
		return measure.RangeMilliMeter/10;
	} else {
		return measure.RangeDMaxMilliMeter/10;
	}
}

