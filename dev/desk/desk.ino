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

class LedDriverControlService {
	int	remoteValue;
	int	value;
public:

	bool	takeInitiative(){
		if(remoteValue==value){
			return false;
		}
		krf.packet.debug.level=value;
	}
	void ack(){
		remoteValue=krf.packet.debug.level;
		show();
	}

	void add(int v){
		value+=v;
		if(v<0)
			value=0;
		if(v>=255)
			value=255;
	}
	void	show(){
		Serial.print("v:	");
		Serial.print(value);
		Serial.print("	rv:");
		Serial.print(remoteValue);
		Serial.println();
	}
};
LedDriverControlService 	ldrvs;
KitchenSensorDebugService	kss;

KChannel channel_kitchen(krf, KRF_ADDR::KITCHEN_SENSOR,1);
KChannelTx channel_leddrv(krf, KRF_ADDR::KITCHEN_STRIP,2);

void setup() {
	Serial.begin(115200);
	Serial.println("# this is: " __FILE__);
	krf.begin();
	channel_kitchen.init();
	channel_leddrv.init();
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
		channel_leddrv.service_rx(SERVICE_KITCHEN, ldrvs);
	});

	channel_leddrv.service_tx(SERVICE_KITCHEN, ldrvs);

	if(Serial.available()){
		char c=Serial.read();
		switch(c){
		case 'v':
			ldrvs.add(1);
		break;
		case 'c':
			ldrvs.add(-1);
		break;
		default:
			break;
		}
		ldrvs.show();

	}
}
