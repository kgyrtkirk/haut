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

#define WRITE	1
#define ERROR	4
template<uint8_t S>
class FwFragments{
	uint8_t		page;
	uint8_t		offset;
	uint8_t		opcode;
	uint8_t		state;
	uint8_t		content[S];

	KRF::Packet	&packet;
public:
	FwFragments(KRF::Packet	&_p) : packet(_p) {
//
//		packet.fw.application=81;
//		packet.fw.offset=9;
//		packet.fw.opcode=1;
	};

	bool	takeInitiative(){
		if(opcode==0)
			return false;
		if(opcode==WRITE){
			packet.fw.opcode=opcode;
			packet.fw.page=page;
			packet.fw.offset=offset;
			memcpy(packet.fw.content,content+packet.fw.offset,sizeof(packet.fw.content));
		}
	}
	void	ack(){
		if(krf.packet.fw.opcode==WRITE){
			offset=krf.packet.fw.offset;
			memcpy(content+offset,krf.packet.fw.content,sizeof(packet.fw.content));
			Serial.print("o:");
			Serial.println(offset);
			if(offset+sizeof(packet.fw.content)>=S){
				Serial.println("full!");
			}
		}

	}
//	FwFragments()  {};
};


FwFragments<128>	fwFrag(krf.packet);
SeqHandler	seqH;

void loop() {
	analogWrite(ledPin, 0);
//	krf.debug();
	if(krf.listen(1000)){
		if(seqH.confirmedWithPrev(krf.packet.hdr.seqId)){
			fwFrag.ack();
			krf.sendTo(KRF_ADDR::DESK0,"pong",4);
		}
//		if(true){//krf.packet.hdr.seqId
//		}
//		int error=0;
//		int i;
//		for(i=0;i<16;i++){
//			if(krf.packet.fw.content[i]==i){
//				error++;
//			}
//		}
//if(error==0){
//		Serial.println("!");
//}else{
//	Serial.println("E");
}
//		Serial.println("!");


}
