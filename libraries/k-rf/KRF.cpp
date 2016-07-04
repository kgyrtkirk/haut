#include "KRF.h"

//KRF::Packet		KRF::packet;

void KRF::listenTo(uint8_t idx, uint32_t _myAddr) {
	radio.openReadingPipe(idx, _myAddr);
}
void KRF::begin() {
	radio.begin();
	radio.setPALevel(RF24_PA_MAX);
//	radio.setCRCLength(RF24_CRC_16);
//	radio.setDataRate(RF24_250KBPS);
	radio.openWritingPipe( myAddr);
	radio.startListening();
	radio.setRetries(2,3);
}
void KRF::send() {
	radio.stopListening();
    if (!radio.write( &packet, sizeof(packet) )){
      Serial.println(F("# failed0"));
    }
	radio.startListening();
	radio.setRetries(15,15);
}
bool KRF::listen(uint16_t timeout) {

	unsigned long deadline = micros() + timeout;

	while (!radio.available() && micros() < deadline) {
		delayMicroseconds(100);
	}
	if(radio.available()){
//        unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
//		Serial.print(F("."));
        radio.read( &packet, sizeof(packet) );
//        unsigned long end_time = micros();
//
//         Spew it
//        Serial.print(F("Sent "));
//        Serial.print(start_time);
//        Serial.print(F(", Got response "));
//        Serial.print(got_time);
//        Serial.print(F(", Round-trip delay "));
//        Serial.print(end_time-start_time);
//        Serial.println(F(" microseconds"));
        return true;
	}
	return false;

}


void KRF::sendTo(const uint32_t dest,const char*packet0,uint8_t len) {
	packet.hdr.source=dest;
	radio.stopListening();
    if (!radio.write( &packet, sizeof(packet) )){
      Serial.println(F("# failed"));
    }
	radio.startListening();
}


void KRF::debug(){
	radio.printDetails();
	Serial.print("x	T:");
	Serial.print(packet.state.temp);
	Serial.print("	H:");
	Serial.print(packet.state.hum);
	Serial.print("	PIR:");
	Serial.print(packet.state.pir);
	Serial.print("	L:");
	Serial.print(packet.state.lum);
	Serial.print("	S:");
	Serial.print(packet.state.strip_bright);
	Serial.println();
}
