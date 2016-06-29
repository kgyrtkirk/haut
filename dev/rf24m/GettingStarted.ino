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

#include "KChannel.h"
FlashUpdateService<128> fwFrag(krf.packet);
KChannel channel(krf, KRF_ADDR::DESK0);
KChannelTx ch2(krf, KRF_ADDR::KITCHEN_SENSOR);


void setup() {
	fwFrag.init();
	Serial.begin(115200);
	Serial.println(KRF_ADDR::KITCHEN_STRIP);
	fdevopen(&my_putc, 0);
	krf.begin();
	krf.listenTo(1, KRF_ADDR::DESK0);

}


uint8_t freeWheel = 0;
void loop() {
	fwFrag.swapOpportunity();
	if (krf.listen(1000)) {
		channel.service_rx(SERVICE_FW, fwFrag);
	} else {
		if (ch2.connected()) {

		} else {
			if (!freeWheel) {
				ch2.connect();
			}
		}
		freeWheel++;
	}
}
