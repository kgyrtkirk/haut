#include <SPI.h>
#include "RF24.h"
#include <KRF.h>

#include "HautCore.h"
#include "Arduino.h"
#include "KRF.h"
#include "FlashUpdateService.h"
#include "KChannel.h"
#include "wiring_private.h"

// fet is connected to Timer1 / OC1A
#define	LED_PIN	9

KRF			krf(7,8,KITCHEN_STRIP);


class Fader {
	uint8_t	value;
	uint8_t	skips;
public:
	uint8_t	target;
	uint8_t	skipCount;

	Fader(){
		value=0;
		target=0;
		skips=0;
		skipCount=0;
	}
	void init() {
		TCCR2B = (4<<CS20);
		TIMSK2 = 1<<TOIE2;
	}
	void isr(){
		if(skips) {
			skips--;
			return;
		}
		skips=skipCount;
		if(value==target){
//			if(value==0)
//				target=255;
//			else
//				target=0;
			return;
		}
		if(value<target){
			value++;
		}else{
			value--;
		}
		analogWrite(LED_PIN, value);
	}
};

Fader	fader;
ISR(TIMER2_OVF_vect){
	fader.isr();
}

HautCore hc(krf);

class KitchenStripService {
public:
	void init(){
	}
	void ack(){
		showState(krf.packet.kitchen);
		if(krf.packet.kitchen.state.pir) {
			fader.target=255;
			fader.skipCount=0;
//			analogWrite(LED_PIN, 100);
		}else{
			fader.target=0;
			fader.skipCount=3;
//			analogWrite(LED_PIN, 0);
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
	fader.init();
	fwFrag.init();
	Serial.begin(115200);
	Serial.println("# this is:  " __FILE__);
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
