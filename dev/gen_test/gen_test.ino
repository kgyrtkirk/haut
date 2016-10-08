#include "Arduino.h"
//The setup function is called once at startup of the sketch

#include <IRremote.h>

 const int hallPin = 5;     // the number of the hall effect sensor pin
 const int ledPin =  13;     // the number of the LED pin

//#define HALL
//#define RECV
#define XDHT
#ifdef HALL
#ifdef HALLV1
/*
Arduino Hall Effect Sensor Project
by Arvind Sanjeev
Please check out  http://diyhacking.com for the tutorial of this project.
DIY Hacking
*/

 volatile byte half_revolutions;
 unsigned int rpm;
 unsigned long timeold;

 void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
 {
   half_revolutions++;
//   Serial.println("detect");
 }


 void setup()
 {
   Serial.begin(9600);
   attachInterrupt(0, magnet_detect, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)
   half_revolutions = 0;
   rpm = 0;
   timeold = 0;
 }
 void loop()//Measure RPM
 {
	   Serial.println(half_revolutions);
   if (half_revolutions >= 20) {
     rpm = 30*1000/(millis() - timeold)*half_revolutions;
     timeold = millis();
     half_revolutions = 0;
     //Serial.println(rpm,DEC);
   }
 }
#else
 // constants won't change. They're used here to set pin numbers:
 // variables will change:
 int hallState = 0;          // variable for reading the hall sensor status

 void setup() {
	   Serial.begin(115200);
   // initialize the LED pin as an output:
   pinMode(ledPin, OUTPUT);
   // initialize the hall effect sensor pin as an input:
   pinMode(hallPin, INPUT);
 }

 void loop(){
   // read the state of the hall effect sensor:
   hallState = digitalRead(hallPin);

   if (hallState == LOW) {
     // turn LED on:
     digitalWrite(ledPin, HIGH);
   }
   else {
     // turn LED off:
     digitalWrite(ledPin, LOW);
   }
 }
#endif


#endif


#ifdef RECV
#ifdef RECV_V0
int RECV_PIN = 6;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
//  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}
  IRsend irsend;

void loop() {
  if (irrecv.decode(&results)) {
//	    Serial.println(results.decode_type, HEX);
//	    Serial.println(NEC, HEX);
//	    Serial.println(results.bits, HEX);
//
//	    Serial.println(results.value, HEX);
	    irsend.sendNEC(0x20DFC03F,32);
	    irrecv.enableIRIn();

	    irrecv.resume(); // Receive the next value

//    20DF40BF
  }

}
#else

IRsend irsend;

void setup()
{
	  Serial.begin(9600);
}

int khz=38;
unsigned int Signal_0_0[] = {9000,4500,560,560,560,560,560,1690,560,560,560,560,560,560,560,560,560,560,560,1690,560,1690,560,560,560,1690,560,1690,560,1690,560,1690,560,1690,560,560,560,560,560,560,560,1690,560,560,560,560,560,560,560,560,560,1690,560,1690,560,1690,560,560,560,1690,560,1690,560,1690,560,1690,560};


void loop() {
		    Serial.println(NEC, HEX);
#ifndef TLED
//		    irsend.sendRaw(Signal_0_0, sizeof(Signal_0_0)/sizeof(int), khz); //AnalysIR Batch Export (IRremote) - RAW

		    irsend.sendNEC(0x20DFC03F,32);
		    	delay(1000); //5 second delay between each signal burst
#else
		    		  pinMode(3, OUTPUT);
//	for (int i = 0; i < 3; i++) {
		    digitalWrite(3, HIGH);


////		irsend.sendSony(0xa90, 12);
		delay(100);
//	    irsend.sendNEC(0xffffffff,20);
	    digitalWrite(3, LOW);
		delay(100);
//	}
#endif
}
#endif
#endif

#ifdef XDHT
#include <DHT.h>
#include <Wire.h>

DHT 		dht(2, DHT22);
const int pirPin = 4;
const int LED_PIN = 9;

void setup()
{
	  Serial.begin(9600);
	   pinMode(pirPin, INPUT);
	   pinMode(LED_PIN, OUTPUT);
	  dht.begin();
}

uint8_t	value=0;

void loop() {
	Serial.print("tem:");
		Serial.println(1.1f);
		Serial.println(dht.readTemperature());
	Serial.print("hum:");
	Serial.println(dht.readHumidity());
	Serial.print("pir:");
	if(digitalRead(pirPin)){
	Serial.println("+");
	}else{
		Serial.println("-");

	}
	analogWrite(LED_PIN, value);
	if(value==0)
		value=1;
	else
		value<<=1;
//    digitalWrite(ledPin, digitalRead(pirPin));

	delay(1000);
	   // initialize the hall effect sensor pin as an input:
}

#endif
