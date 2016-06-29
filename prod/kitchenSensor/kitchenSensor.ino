
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

LightMeter	lm(0);
DHT 		dht(9, DHT22);
KRF			krf(7,8,KITCHEN_SENSOR);

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 1;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
/**********************************************************/


byte addresses[][6] = {"1Node","2Node"};

// Used to control whether this node is sending or receiving
bool role = 0;




#define	STR(A)	#A
void setup1() {
  Serial.begin(115200);
  Serial.println(F("*** KS " __FILE__));

  fdevopen( &my_putc, 0);
  printf("hello, world!\n");
  
  Serial.println(F("*** KS " __FILE__ ":" STR(__LINE__)));
  dht.begin();
  Serial.println(F("*** KS " __FILE__ ":" STR(__LINE__)));
  krf.begin();
  Serial.println(F("*** KS " __FILE__ ":" STR(__LINE__)));
//  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
//  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
//  if(radioNumber){
//    radio.openWritingPipe(addresses[1]);
//    radio.openReadingPipe(1,addresses[0]);
//  }else{
//    radio.openWritingPipe(addresses[0]);
//    radio.openReadingPipe(1,addresses[1]);
//  }
//
//  // Start the radio listening for data
//  radio.startListening();
}

#define	pirPin	3

void loop1() {
  
	krf.packet.state.pir=	digitalRead(pirPin);
	krf.packet.state.hum= dht.readHumidity();
	krf.packet.state.temp=dht.readTemperature();
	krf.packet.state.lum=lm.light();

	  Serial.println(F("***  " __FILE__));
	krf.send();
	krf.debug();
	delay(1000);
  

} // Loop


#include "HautCore.h"
#include "Arduino.h"
#include "KRF.h"
#include "FlashUpdateService.h"
#include "KChannel.h"
#include "wiring_private.h"

int ledPin = 9;    // LED connected to digital pin 9

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
KChannelTx channel_kitchen(krf, KRF_ADDR::DESK1);


void setup() {
	fwFrag.init();
	Serial.begin(115200);
	Serial.println("# this is: " __FILE__);
	fdevopen(&my_putc, 0);
	krf.begin();
	krf.listenTo(1, KRF_ADDR::DESK0);
	krf.listenTo(2, KRF_ADDR::DESK1);
	kss.init();
}


uint8_t freeWheel = 0;
void loop() {
	fwFrag.swapOpportunity();
	if (krf.listen(1000)) {
		channel_fw.service_rx(SERVICE_FW, fwFrag);
		channel_kitchen.service_rx(SERVICE_KITCHEN, kss);
	}
//	else
	{
		if(freeWheel==0) {
			// at every ~250ms try to send it
			channel_kitchen.service_tx(SERVICE_KITCHEN, kss);
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
