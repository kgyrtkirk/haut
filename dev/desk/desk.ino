#include <SPI.h>
#include "RF24.h"
#include <KRF.h>

KRF krf(9, 10, KRF_ADDR::DESK1);

#include "Arduino.h"
#include "KRF.h"
#include "KChannel.h"
#include "wiring_private.h"

int ledPin = 9;    // LED connected to digital pin 9


class KitchenSensorDebugService {
public:
	void init(){
	}
	void ack(){
		showState(krf.packet.kitchen);
	}
	void showState(KRF::Packet_Kitchen&packet){
        Serial.print(F("x T:"));
        Serial.print(packet.state.temp);
        Serial.print(F("  H:"));
        Serial.print(packet.state.hum);
        Serial.print(F("  PIR:"));
        Serial.print(packet.state.pir);
        Serial.print(F("  L:"));
        Serial.print(packet.state.lum);
        Serial.print(F("  S:"));
        Serial.print(packet.state.strip_bright);
        Serial.println();
	}

};

KitchenSensorDebugService	kss;

KChannel channel_kitchen(krf, KRF_ADDR::KITCHEN_SENSOR,1);

void setup() {
	Serial.begin(115200);
	Serial.println("# this is: " __FILE__);
	krf.begin();
	channel_kitchen.init();
//	krf.listenTo(1, KRF_ADDR::KITCHEN_SENSOR);
	kss.init();
}
#include "ChaChaPoly.h"


void loop() {
	ChaChaPoly	cp;
	krf.listen(1000,[&] {
        Serial.print("# rx ");
        Serial.print(krf.packet.hdr.seqId);
        Serial.print(" ");
        Serial.println(krf.packet.hdr.destination);
		channel_kitchen.service_rx(SERVICE_KITCHEN, kss);
	});
}
