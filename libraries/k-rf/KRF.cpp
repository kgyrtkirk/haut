#include "KRF.h"
#include "RF24.h"
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
	radio.setAutoAck(true);
//	radio.enableAckPayload();
}

bool KRF::listen(uint16_t timeout) {

	unsigned long deadline = micros() + timeout;

	while (!radio.available() && micros() < deadline) {
		delayMicroseconds(100);
	}
	if(radio.available(&rx_channel)){
//		Serial.print(F("."));
        radio.read( &packet, sizeof(packet) );
        return true;
	}
	return false;

}


void KRF::sendTo(const uint32_t dest) {
	packet.hdr.destination=dest;
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
