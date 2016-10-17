#pragma once
#include "Arduino.h"
#include "IRremote.h"

class KIR {
	IRrecv irrecv;
	IRsend irsend;
public:
	decode_results results;

	KIR(uint8_t recvPin ) : irrecv(recvPin){
	}

	void init() {
		irrecv.enableIRIn(); // Start the receiver
	}

	void loop() {
		if (irrecv.decode(&results)) {
			Serial.println(results.decode_type, HEX);
//         Serial.println(NEC, HEX);
			Serial.println(results.bits, HEX);
//
			Serial.println(results.value, HEX);
			irsend.sendNEC(0x20DFC03F, 32);
			irrecv.enableIRIn();

			irrecv.resume(); // Receive the next value

//    20DF40BF
		}

	}

};
