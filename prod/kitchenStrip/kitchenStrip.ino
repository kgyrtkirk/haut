#include <SPI.h>
#include "RF24.h"
#include <KRF.h>

#include "HautCore.h"
#include "Damper.h"
#include "Arduino.h"
#include "KRF.h"
#include "FlashUpdateService.h"
#include "KChannel.h"
#include "wiring_private.h"
#include "DelayControlValue.h"
#include "FadeController.h"

// fet is connected to Timer1 / OC1A
#define	LED_PIN	9

KRF			krf(7,8,KITCHEN_STRIP);


Fader<3>	fader(9);

ISR(TIMER2_OVF_vect){
	fader.isr();
}


HautCore hc(krf);


class KitchenStripService {
	FaderTargetValue	newTarget;
	Damper				dampedLightSense;
public:
	KitchenStripService() : dampedLightSense(0.8) {

	}

	void init(){
		newTarget.skipCount=3;
		newTarget.target=0;
		fader.dcv.command(0,0,newTarget);
	}
	void ack(){
		dampedLightSense.update(krf.packet.kitchen.state.lum);
//		showState(krf.packet.kitchen);
		if(krf.packet.kitchen.state.pir) {
			if(fader.dcv.getActiveState() == 0 && dampedLightSense.getValue() > 50){
				// do nothing...there is enough light there
			}else{
				newTarget.skipCount=0;
				newTarget.target=255;
				fader.dcv.command(1,millis()+60000,newTarget);
			}
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

class DebugLedService {
public:
	void ack(){
		FaderTargetValue	target;
		target.skipCount=0;
		target.target=krf.packet.debug.level;
		fader.dcv.command(2,millis()+180000,target);
	}

};
FlashUpdateService<128> fwFrag(krf.packet);
KitchenStripService	kss;
DebugLedService dls;

KChannel channel_fw(krf, KRF_ADDR::DESK0,1);
KChannel channel_kitchen(krf, KRF_ADDR::KITCHEN_SENSOR,2);
KChannel channel_debug(krf, KRF_ADDR::DESK1,3);


void setup() {
	fader.init();
	fwFrag.init();
	Serial.begin(115200);
	Serial.println("# this is:  " __FILE__);
	krf.begin();
	channel_fw.init();
	channel_kitchen.init();
	channel_debug.init();
	kss.init();
}


uint8_t freeWheel = 0;
void loop() {
	fwFrag.swapOpportunity();
	krf.listen(1000,[&] {
		channel_fw.service_rx(SERVICE_FW, fwFrag);
		channel_kitchen.service_rx(SERVICE_KITCHEN, kss);
		channel_debug.service_rx(SERVICE_KITCHEN, dls);
	});
	freeWheel++;
}
