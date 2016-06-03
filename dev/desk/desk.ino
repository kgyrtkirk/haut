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
	Serial.println("hello, world!");
	krf.begin();
	krf.listenTo(1, KRF_ADDR::KITCHEN);
	krf.listenTo(2, KRF_ADDR::KITCHEN_STRIP);
//	Serial.println(F("*** KS " __FILE__ ":" STR(__LINE__)));
	Serial.println("READY");

}

#define WRITE	1
#define READ	2
#define ERROR	4
template<size_t S>
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
		page=0;
		offset=0;
		opcode=READ;
	};

	bool	takeInitiative(){
		if(opcode==0)
			return false;
		if(opcode==WRITE || opcode==READ){
			uint8_t i;

			packet.fw.opcode=opcode;
			packet.fw.page=page;
			packet.fw.offset=offset;
			memcpy(packet.fw.content,content+packet.fw.offset,sizeof(packet.fw.content));
			return true;
		}
		return false;
	}
	void	ack(){
		if(packet.fw.opcode == ERROR){
			Serial.println("error!");
			opcode=0;
		}
		if(opcode==WRITE){
			offset+=sizeof(packet.fw.content);
			if(offset>=S){
				Serial.println("# sent!");
				opcode=0;
			}
		}
		if(opcode==READ){
//			Serial.println(packet.fw.offset);
			memcpy(content+packet.fw.offset,packet.fw.content,sizeof(packet.fw.content));
			offset+=sizeof(packet.fw.content);
			if(offset>=S){
				uint8_t	i;
				for(i=0;i<S;i++){
					Serial.print(content[i]>>4,16);
					Serial.print(content[i]&0xf,16);
				}
				Serial.println();
				Serial.println("# sent!");
				opcode=0;
			}
		}


	}
	void setRead(uint16_t o){
		Serial.print("# >R @");
		Serial.println(o);
		page=o/S;
		offset=0;
		opcode=READ;
	}

//	FwFragments()  {};
};


FwFragments<128>	fwFrags(krf.packet);
SeqHandler			seqH;
#include "KChannel.h"

KChannelTx			channel(krf,KRF_ADDR::KITCHEN_STRIP);
int cnt=5;
void loop() {

//	krf.debug();
	if (krf.listen(1000)) {
		if(channel.isValid()){
			// dispatch to application
			if(channel.dispatch()){
				fwFrags.ack();
			}
			Serial.println("# !");
		}
	}

	if(channel.connected()) {
		if(fwFrags.takeInitiative()){
			channel.send();
		}else{
			if(Serial.available()){
				if(Serial.read() == 'R'){
					uint16_t o=Serial.parseInt();
					fwFrags.setRead(o);
				}
			}
		}
	}else{
		channel.connect();
	}
//		krf.packet.hdr.seqId=seqH.get();
//		Serial.println(krf.packet.hdr.seqId);
//		krf.packet.hdr.source=KRF_ADDR::KITCHEN_STRIP;
//		krf.sendTo(KRF_ADDR::KITCHEN_STRIP, "hello", 5);
//	}
////	krf.sendTo(KRF_ADDR::KITCHEN, "hello", 5);
//	cnt--;
//	if(cnt==0){
//		cnt=30;
//	}

}

