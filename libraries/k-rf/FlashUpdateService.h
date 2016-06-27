#pragma once

#include "KRF.h"
#include "avr/pgmspace.h"
#include "optiboot-service.h"
#include "avr/wdt.h"


template<uint8_t S>
class FlashUpdateService{
	uint16_t	newFwLength;
	uint8_t		page;
	uint8_t		offset;
	uint8_t		opcode;
	uint8_t		state;
	uint8_t		content[S];
	uint8_t	written;
	KRF::Packet	&packet;
public:
	FlashUpdateService(KRF::Packet	&_p) : packet(_p),newFwLength(0) {
	};

	void	ack(){
		if(packet.fw.opcode==FW_OPCODE_WRITE){
			offset=packet.fw.offset;
			uint16_t	i;
			if(offset==0)
				written=0;
			memcpy(content+offset,packet.fw.content,sizeof(packet.fw.content));
			Serial.print("o:");
			Serial.println(offset);
			if(offset+sizeof(packet.fw.content)>=S){
				Serial.println("full!");
				if(!written){
					written=1;
				uint16_t dstAddr=packet.fw.page;
				dstAddr*=128;
				if(OTA_FLASH_START  <= dstAddr && dstAddr < OTA_FLASH_END){
					optiboot_service('F',content,dstAddr,sizeof(content));
				}else{
					// notify?
				}
				}
			}
		}
		if(packet.fw.opcode==FW_OPCODE_READ){
			uint16_t	o;
			uint8_t		i;
			o=packet.fw.page;
			o*=128;
			o+=packet.fw.offset;
			for(i=0;i<sizeof(packet.fw.content);i++){
				packet.fw.content[i]=pgm_read_byte_near(o+i);
			}
		}
		if(packet.fw.opcode==FW_OPCODE_SWAP){
			Serial.println("/\/\/ swap!");
			newFwLength = packet.fw.page;
			newFwLength*=128;
		}

	}

	void swapOpportunity(){
		if(newFwLength){
			delay(1000);
			wdt_reset();
			optiboot_service('X',content,newFwLength,sizeof(content));
		}

	}
//	FwFragments()  {};
};
