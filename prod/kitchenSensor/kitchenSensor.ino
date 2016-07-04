
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"
#include <DHT.h>
#include <LightMeter.h>
#include <KRF.h>

#include "HautCore.h"
#include "Arduino.h"
#include "KRF.h"
#include "FlashUpdateService.h"
#include "KChannel.h"
#include "wiring_private.h"

#define	pirPin	3

LightMeter	lm(0);
DHT 		dht(9, DHT22);
KRF			krf(7,8,KITCHEN_SENSOR);



HautCore hc(krf);

class KitchenSensorService {
public:
	void init(){
		dht.begin();
	}
	void ack(){
	}
	bool takeInitiative(){
		krf.packet.kitchen.state.pir=	digitalRead(pirPin);
		krf.packet.kitchen.state.hum= dht.readHumidity();
		krf.packet.kitchen.state.temp=dht.readTemperature();
		krf.packet.kitchen.state.lum=lm.light();
		return true;
	}

};

int my_putc(char c, FILE *t) {
	Serial.write(c);
}
//#include "bootloaders/optiboot/optiboot.h"

FlashUpdateService<128> fwFrag(krf.packet);
KitchenSensorService	kss;

KChannel channel_fw(krf, KRF_ADDR::DESK0);
KChannelTx channel_kitchen(krf, KRF_ADDR::KITCHEN_STRIP);
KChannelTx channel_debug(krf, KRF_ADDR::DESK1);


void setup() {
	fwFrag.init();
	Serial.begin(115200);
	Serial.println("# this is: " __FILE__);
	fdevopen(&my_putc, 0);
	krf.begin();
	krf.listenTo(1, KRF_ADDR::DESK0);
	krf.listenTo(2, KRF_ADDR::KITCHEN_STRIP);
	krf.listenTo(3, KRF_ADDR::DESK1);
	kss.init();
}


uint8_t freeWheel = 0;
void loop() {
	fwFrag.swapOpportunity();
	unsigned long deadline = micros() + 1000;

	while (deadline > micros() && krf.listen(1000)) {
		channel_fw.service_rx(SERVICE_FW, fwFrag);
		channel_kitchen.service_rx(SERVICE_KITCHEN, kss);
		channel_debug.service_rx(SERVICE_KITCHEN, kss);
	}
//	else
	{
		if(freeWheel==128) {
			// at every ~250ms try to send it
			channel_kitchen.service_tx(SERVICE_KITCHEN, kss);
		}
		if(freeWheel==0){
			channel_debug.service_tx(SERVICE_KITCHEN, kss);
		}
	}
//		if (ch2.connected()) {
//
//		} else {
//			if (!freeWheel) {
//				ch2.connect();
//			}
//		}
		freeWheel++;
}
