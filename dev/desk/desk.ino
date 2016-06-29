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
        Serial.print("x T:");
        Serial.print(packet.state.temp);
        Serial.print("  H:");
        Serial.print(packet.state.hum);
        Serial.print("  PIR:");
        Serial.print(packet.state.pir);
        Serial.print("  L:");
        Serial.print(packet.state.lum);
        Serial.print("  S:");
        Serial.print(packet.state.strip_bright);
        Serial.println();
	}

};

int my_putc(char c, FILE *t) {
	Serial.write(c);
}
//#include "bootloaders/optiboot/optiboot.h"

KitchenSensorDebugService	kss;

KChannel channel_kitchen(krf, KRF_ADDR::KITCHEN_SENSOR);

void setup() {
	Serial.begin(115200);
	Serial.println("# this is: " __FILE__);
	fdevopen(&my_putc, 0);
	krf.begin();
	krf.listenTo(1, KRF_ADDR::KITCHEN_SENSOR);
	kss.init();
}

void loop() {
	if (krf.listen(1000)) {
        Serial.println("# rx");
		channel_kitchen.service_rx(SERVICE_KITCHEN, kss);
	}
}
