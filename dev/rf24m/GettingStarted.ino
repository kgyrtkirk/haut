#include "Arduino.h"
#include "KRF.h"

#include "wiring_private.h"

int ledPin = 9;    // LED connected to digital pin 9

KRF krf(7, 8, KRF_ADDR::KITCHEN_STRIP);

int my_putc(char c, FILE *t) {
	Serial.write(c);
}
//#include "bootloaders/optiboot/optiboot.h"

void setup() {
	Serial.begin(115200);
	Serial.println(KRF_ADDR::KITCHEN_STRIP);
	fdevopen(&my_putc, 0);
	krf.begin();
	krf.listenTo(1, KRF_ADDR::DESK0);
}
void loop() {
//	krf.debug();
	if(krf.listen(1000)){
		int error=0;
		int i;
		for(i=0;i<16;i++){
			if(krf.packet.fw.content[i]==i){
				error++;
			}
		}
if(error==0){
		Serial.println("!");
}else{
	Serial.println("E");
}
//		Serial.println("!");
		krf.sendTo(KRF_ADDR::DESK0,"pong",4);
	}

	analogWrite(ledPin, 0);

}
