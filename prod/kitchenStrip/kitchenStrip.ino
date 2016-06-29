#include <SPI.h>
#include "RF24.h"
#include <KRF.h>

#include "HautCore.h"
#include "Arduino.h"
#include "KRF.h"
#include "FlashUpdateService.h"
#include "KChannel.h"
#include "wiring_private.h"

#define	LED_PIN	9

KRF			krf(7,8,KITCHEN_STRIP);



HautCore hc(krf);

class KitchenStripService {
public:
	void init(){
	}
	void ack(){
		showState(krf.packet.kitchen);
		if(krf.packet.kitchen.state.pir) {
			analogWrite(LED_PIN, 100);
		}else{
			analogWrite(LED_PIN, 0);
		}
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

FlashUpdateService<128> fwFrag(krf.packet);
KitchenStripService	kss;

KChannel channel_fw(krf, KRF_ADDR::DESK0);
KChannel channel_kitchen(krf, KRF_ADDR::KITCHEN_SENSOR);


void setup() {
	fwFrag.init();
	Serial.begin(115200);
	Serial.println("# this is: +++ " __FILE__);
	fdevopen(&my_putc, 0);
	krf.begin();
	krf.listenTo(1, KRF_ADDR::DESK0);
	krf.listenTo(2, KRF_ADDR::KITCHEN_SENSOR);
	kss.init();
}


uint8_t freeWheel = 0;
void loop() {
	fwFrag.swapOpportunity();
	if (krf.listen(1000)) {
		channel_fw.service_rx(SERVICE_FW, fwFrag);
		channel_kitchen.service_rx(SERVICE_KITCHEN, kss);
	}
	freeWheel++;
}
