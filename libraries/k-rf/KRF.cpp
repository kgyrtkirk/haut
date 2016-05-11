#include "KRF.h"

void KRF::listenTo(uint8_t idx, const char*_myAddr) {
	radio.openReadingPipe(idx, (const uint8_t*) _myAddr);
}
void KRF::begin() {
	radio.begin();
	radio.setPALevel(RF24_PA_MAX);
	radio.openWritingPipe((const uint8_t*) myAddr);
	radio.startListening();
}
void KRF::send() {
	radio.stopListening();
    if (!radio.write( &state, sizeof(state) )){
      Serial.println(F("failed"));
    }
	radio.startListening();
}
void KRF::listen(uint32_t timeout) {

	unsigned long deadline = micros() + timeout;

	while (!radio.available() && micros() < deadline) {
		delayMicroseconds(100);
	}
	while(radio.available()){
//        unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
		Serial.print(F("."));
        radio.read( &state, sizeof(state) );
//        unsigned long end_time = micros();

        // Spew it
//        Serial.print(F("Sent "));
//        Serial.print(start_time);
//        Serial.print(F(", Got response "));
//        Serial.print(got_time);
//        Serial.print(F(", Round-trip delay "));
//        Serial.print(end_time-start_time);
//        Serial.println(F(" microseconds"));

	}

}

void KRF::debug(){
//	radio.printDetails();
	Serial.print("x	T:");
	Serial.print(state.temp);
	Serial.print("	H:");
	Serial.print(state.hum);
	Serial.print("	PIR:");
	Serial.print(state.pir);
	Serial.print("	L:");
	Serial.print(state.lum);
	Serial.println();
}
