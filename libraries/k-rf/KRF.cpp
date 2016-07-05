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
	Serial.println();
}
