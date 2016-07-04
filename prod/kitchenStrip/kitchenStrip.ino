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

struct	FaderTargetValue {
	uint8_t	target;
	uint8_t	skipCount;
};



template<class VT,size_t CNT>
class DelayControlValue {
	struct DelayedValue{
		VT			value;
		uint64_t	until;
	};
	DelayedValue	dv[CNT];
public:
	void command(uint8_t priority,uint64_t until,const VT&value) {
		if(priority>=CNT){
			return;
		}
		dv[priority].value=value;
		dv[priority].until=until;
	}

	VT getActiveValue(){
		uint8_t	i;
		uint64_t	now=millis();
		for(i=CNT-1;i>0;i--){
			if(dv[i].until > now){
				goto return_active;
//				return dv[i].value;
			}
		}
		return_active:
		return dv[i].value;
	}

};
DelayControlValue<FaderTargetValue,2>	dcv;

class Fader {
	uint8_t	value;
	uint8_t	skips;
public:
//	uint8_t	target;
//	uint8_t	skipCount;

	Fader(){
		value=0;
		skips=0;
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
		FaderTargetValue v=dcv.getActiveValue();
//		skipCount=v.skipCount;
//		target=v.target;

		skips=v.skipCount;
		if(value==v.target){
			return;
		}
		if(value<v.target){
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
	FaderTargetValue	newTarget;
public:
	void init(){
		newTarget.skipCount=3;
		newTarget.target=0;
		dcv.command(0,0,newTarget);
	}
	void ack(){
		showState(krf.packet.kitchen);
		if(krf.packet.kitchen.state.pir) {
			newTarget.skipCount=0;
			newTarget.target=255;
			dcv.command(1,millis()+60000,newTarget);
//			fader.target=255;
//			fader.skipCount=0;
//			analogWrite(LED_PIN, 100);
		}else{
//			fader.target=0;
//			fader.skipCount=3;
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
