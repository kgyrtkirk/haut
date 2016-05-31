#include <SPI.h>
#include "RF24.h"
#include <KRF.h>

KRF krf(9, 10, KRF_ADDR::DESK0);

int my_putc(char c, FILE *t) {
	Serial.write(c);
}

#define	STR(A)	#A
void setup() {
	Serial.begin(115200);
	Serial.println(F("*** KS " __FILE__));
	fdevopen(&my_putc, 0);
	printf("hello, world!\n");
	krf.begin();
	krf.listenTo(1, KRF_ADDR::KITCHEN);
	krf.listenTo(2, KRF_ADDR::KITCHEN_STRIP);
	Serial.println(F("*** KS " __FILE__ ":" STR(__LINE__)));

}

int cnt=5;
void loop() {
//	krf.debug();
	if (krf.listen(1000)) {
		int error=0;
		int i;
		for(i=0;i<16;i++){
			if(krf.packet.fw.content[i]!=i){
				error++;
			}
		}
if(error==0){
	Serial.print(sizeof(krf.packet));
	Serial.println("!");
}else{
	Serial.println(error);
}
	}

//	krf.sendTo(KRF_ADDR::KITCHEN, "hello", 5);
	cnt--;
	if(cnt==0){
		cnt=30;
	krf.sendTo(KRF_ADDR::KITCHEN_STRIP, "hello", 5);
	}

}

