#include "Arduino.h"
#include "KRF.h"

#include "wiring_private.h"
#include "optiboot-service.h"
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

#include "avr/pgmspace.h"
#define WRITE	1
#define READ	2
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
		if(opcode==WRITE || opcode==READ){
			packet.fw.opcode=opcode;
			packet.fw.page=page;
			packet.fw.offset=offset;
			memcpy(packet.fw.content,content+packet.fw.offset,sizeof(packet.fw.content));
		}
	}
	void	ack(){
		if(krf.packet.fw.opcode==WRITE){
			offset=krf.packet.fw.offset;
			uint16_t	i;
//			for(i=0;i<sizeof(packet.fw.content);i++){
//				Serial.print(packet.fw.content[i]);
//				Serial.print(",");
//			}
//			Serial.println();

			memcpy(content+offset,krf.packet.fw.content,sizeof(packet.fw.content));
			Serial.print("o:");
			Serial.println(offset);
			if(offset+sizeof(packet.fw.content)>=S){
//				for(i=0;i<S;i++){
//					Serial.print(content[i]>>4,16);
//					Serial.print(content[i]&0xf,16);
//					Serial.print(content[i]);
//					Serial.print(",");
//				}
				Serial.println("full!");
				uint16_t dstAddr=krf.packet.fw.page;
				dstAddr*=128;
				dstAddr+=OTA_FLASH_START;
				optiboot_service('F',content,dstAddr,sizeof(content));
			}
		}
		if(krf.packet.fw.opcode==READ){
			uint16_t	o;
			uint8_t		i;
			o=krf.packet.fw.page;
			o*=128;
			o+=krf.packet.fw.offset;
			for(i=0;i<sizeof(krf.packet.fw.content);i++){
				krf.packet.fw.content[i]=pgm_read_byte_near(o+i);
			}
		}

	}
//	FwFragments()  {};
};


#include "KChannel.h"
FwFragments<128>	fwFrag(krf.packet);
SeqHandler	seqH;
KChannel			channel(krf, KRF_ADDR::DESK0);

void loop() {
	analogWrite(ledPin, 0);
//	krf.debug();
	if(krf.listen(1000)) {
		if(channel.isValid()){
			if(channel.dispatch()) {
				fwFrag.ack();
			}
			channel.send();
		}
//		if(seqH.confirmedWithPrev(krf.packet.hdr.seqId)){
//		}
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
