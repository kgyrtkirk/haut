#include "HautCore.h"
#include "Arduino.h"
#include "KRF.h"
#include "FlashUpdateService.h"

#include "wiring_private.h"
int ledPin = 9;    // LED connected to digital pin 9

KRF krf(7, 8, KRF_ADDR::KITCHEN_STRIP);
HautCore hc(krf);

int my_putc(char c, FILE *t) {
	Serial.write(c);
}
//#include "bootloaders/optiboot/optiboot.h"

uint8_t	clear;

void setup() {
	clear=0;
	wdt_disable();
	wdt_reset();
	wdt_enable(WDTO_1S);
	Serial.begin(115200);
	Serial.println(KRF_ADDR::KITCHEN_STRIP);
	fdevopen(&my_putc, 0);
	krf.begin();
	krf.listenTo(1, KRF_ADDR::DESK0);

}



#include "KChannel.h"
FlashUpdateService<128>	fwFrag(krf.packet);
KChannel			channel(krf, KRF_ADDR::DESK0);
KChannelTx			ch2(krf, KRF_ADDR::KITCHEN_SENSOR);


uint8_t	freeWheel=0;
void loop() {

	wdt_reset();
//	if(clear>=255){
//		micros();
	fwFrag.swapOpportunity();
//	}
//	analogWrite(ledPin, 0);
//	krf.debug();
	if(krf.listen(1000)) {
		clear=0;
		channel.service_rx(SERVICE_FW,fwFrag);
}else{
	if(ch2.connected()){

	}else{
		if(!freeWheel) {
			ch2.connect();
		}
	}
	if(clear<255)
		clear++;
	freeWheel++;
}
//		Serial.println("!");


}
