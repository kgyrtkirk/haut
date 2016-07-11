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
	double	lightAggr=0.0;
	void init(){
	}
	void ack(){
		showState(krf.packet.kitchen);
	}
	void showState(KRF::Packet_Kitchen&packet){
        lightAggr=lightAggr*0.8+0.2*packet.state.lum;

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
        Serial.print(F("  LA:"));
        Serial.print(lightAggr);
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
		return true;
	}
	void ack(){
		remoteValue=krf.packet.debug.level;
		show();
	}

	void add(int v){
		value+=v;
		if(value<0)
			value=0;
		if(value>=255)
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

uint8_t	freeWheel=0;
void loop() {
	krf.listen(500,[&] {
		channel_kitchen.service_rx(SERVICE_KITCHEN, kss);
		channel_leddrv.service_rx(SERVICE_KITCHEN, ldrvs);
//        Serial.print("# rx ");
        Serial.print(krf.packet.hdr.seqId);
        Serial.print(" ");
        Serial.println(krf.packet.hdr.destination);
	});

	if(freeWheel%32)
	channel_leddrv.service_tx(SERVICE_KITCHEN, ldrvs);

	freeWheel++;
	while(Serial.available()){
		char c=Serial.read();
		switch(c){
		case 'v':
			ldrvs.add(1);
		break;
		case 'b':
			ldrvs.add(8);
		break;
		case 'n':
			ldrvs.add(32);
		break;
		case 'c':
			ldrvs.add(-1);
		break;
		case 'x':
			ldrvs.add(-8);
		break;
		case 'z':
			ldrvs.add(-32);
		break;
		default:
			break;
		}
		ldrvs.show();

	}
}
