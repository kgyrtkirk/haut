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
	Serial.println(F("# " __FILE__));
	fdevopen(&my_putc, 0);
	krf.begin();
	krf.listenTo(1, KRF_ADDR::KITCHEN);
	krf.listenTo(2, KRF_ADDR::KITCHEN_STRIP);
//	Serial.println(F("*** KS " __FILE__ ":" STR(__LINE__)));
	Serial.println("READY");

}

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
		opcode=FW_OPCODE_READ;
	};

	bool	takeInitiative(){
		if(opcode==0)
			return false;
		if(opcode==FW_OPCODE_WRITE || opcode==FW_OPCODE_READ || opcode == FW_OPCODE_SWAP){
			uint8_t i;
			packet.ahdr.application=SERVICE_FW;
			packet.fw.opcode=opcode;
			packet.fw.page=page;
			packet.fw.offset=offset;
			if(offset>=S){
				Serial.println("# overflow");
			}
			memcpy(packet.fw.content,content+packet.fw.offset,sizeof(packet.fw.content));
			return true;
		}
		return false;
	}
	void	ack(){
		if(packet.fw.opcode == FW_OPCODE_ERROR){
			Serial.println("error!");
			opcode=0;
		}
		if(opcode==FW_OPCODE_WRITE){
			offset+=sizeof(packet.fw.content);
			if(offset>=S){
				Serial.println("# sent!");
				Serial.println("WRITE-OK");
				opcode=0;
				return;
			}
		}
		if(opcode==FW_OPCODE_READ){
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
		if(opcode==FW_OPCODE_SWAP){
			opcode=0;
		}
	}
	void setRead(uint16_t o){
		Serial.print("# >R @");
		Serial.println(o);
		page=o/S;
		offset=0;
		opcode=FW_OPCODE_READ;
	}
	void setWrite(uint16_t o,uint8_t*b){
		Serial.print("# >W @");
		Serial.println(o);
		page=o/S;
		offset=0;
		opcode=FW_OPCODE_WRITE;
		memcpy(content,b,sizeof(content));
	}
	void setSwap(uint16_t o){
		Serial.print("# >SWAP");
		Serial.println(o);
		page=o/S;
		offset=0;
		opcode=FW_OPCODE_SWAP;
	}

//	FwFragments()  {};
};


FwFragments<128>	fwFrags(krf.packet);
SeqHandler			seqH;
#include "KChannel.h"

KChannelTx			channel_fw(krf,KRF_ADDR::KITCHEN_STRIP);
int cnt=5;


uint8_t	parseB16(uint8_t c){
	if('0'<=c && c<='9')
		return c-'0';
	if('a'<=c && c<='f')
		return c-'a'+10;
	if('A'<=c && c<='F')
		return c-'A'+10;
	return 0xb;
}

void loop() {

//	krf.debug();
	if (krf.listen(1000)) {
		if(channel_fw.isValid()){
			// dispatch to application
			if(channel_fw.dispatch()){
				fwFrags.ack();
			}
			Serial.println("# !");
		}
	}

	if(channel_fw.connected()) {
		if(fwFrags.takeInitiative()){
			channel_fw.send();
		}else{
			if(Serial.available()){
				char c=Serial.read();
				if(c == 'R'){
					uint16_t o=Serial.parseInt();
					fwFrags.setRead(o);
				}
				if(c == 'W'){
//					Serial.println("# W");
					uint16_t o=Serial.parseInt();
					uint8_t	b[128];
					Serial.print("# W@");
					Serial.println(o);
					Serial.find(' ');// throw away space
					size_t i;
					for(i=0;i<sizeof(b);i++){
//						Serial.print("#");
//						Serial.println(i);
						uint8_t	bb[2];
						Serial.readBytes(bb,2);
						b[i]=(parseB16(bb[0])<<4)|parseB16(bb[1]);
					}
//					Serial.println("# setW");
					fwFrags.setWrite(o,b);
//					fwFrags.setWrite(o);
				}
				if(c == 'S'){
					Serial.print("# S?");
					uint16_t o=Serial.parseInt();
					Serial.println(o);
					if(o==1234){
						uint16_t o=Serial.parseInt();
						fwFrags.setSwap(o);
					}
				}
			}
		}
	}else{
		channel_fw.connect();
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

