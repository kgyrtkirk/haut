/*
 * KChannel.h
 *
 *  Created on: Jun 2, 2016
 *      Author: kirk
 */

#pragma once

#include "SeqHandler.h"
#include "KRF.h"

class KChannel {
	SeqHandler	seqH;
public:
	KRF			&krf;
	uint16_t	addr;
	uint8_t		errors;
	bool		up;

	KChannel(KRF &_krf,uint16_t _addr) : 		krf(_krf), addr(_addr),errors(0), up(false) {
	}

	bool isValid(){
		if(krf.packet.hdr.source != krf.myAddr){
			return false;
		}
		if(krf.packet.hdr.seqId == 0)
			return true;
		if(krf.packet.hdr.seqId == 1) {
			// decode with new IV; if success change current
			seqH.reset();
		}else{
			// decode with current
		}
		if(seqH.confirmedWithPrev(krf.packet.hdr.seqId)){
			return true;
		}
		return false;
	}

	bool dispatch(){
		return krf.packet.hdr.seqId!=0;
	}

	void send(){
		if(krf.packet.hdr.seqId==0) {
			// send IV
		}
		krf.packet.hdr.source=addr;
		krf.sendTo(addr, "pong", 5);
//
//		krf.sendTo(KRF_ADDR::DESK0,"pong",4);
	}

	// XXX: code duplication
	template<class T>
	void service_rx(uint8_t serviceId,T&service){
		if(krf.packet.ahdr.application != serviceId){
			return;
		}
		if(isValid()){
			Serial.println("Valid");
			if(dispatch()) {
				Serial.println("Disp!");
					service.ack();
//				else
//					channel.reset();
			}
			send();
		}
	}
};

class KChannelTx : public KChannel{
	SeqHandler	seqH;
public:
	KChannelTx(KRF &_krf,uint16_t _addr) : 		KChannel(_krf,_addr) {
	}

	bool isValid(){
		if(krf.packet.hdr.source != krf.myAddr){
			return false;
		}
		if(krf.packet.hdr.seqId == 0){
			// load IV
			up=true;
			return true;
		}
		if(seqH.confirmed(krf.packet.hdr.seqId)){
			errors=0;
			return true;
		}else{
			errors++;
			if(errors>8)
				up=false;
			return false;
		}
	}

	bool dispatch(){
		return krf.packet.hdr.seqId!=0;
	}

	void send(){
		errors++;
		if(errors>8)
			up=false;
		krf.packet.hdr.seqId=seqH.get();
		krf.packet.hdr.source=0xbdbd;
		krf.sendTo(addr, "pong", 5);
	}

	bool connected(){
		return up;
	}
	void connect(){
		Serial.println("# connect");
		seqH.reset();
		errors=0;
		krf.packet.hdr.seqId=0;
		krf.packet.hdr.source=0xbdbd;
		krf.sendTo(addr, "pong", 5);

	}

	// XXX: code duplication
	template<class T>
	inline void service_rx(uint8_t serviceId,T&service){
		if(isValid()){
			Serial.println("Valid");
			if(dispatch()) {
				if(krf.packet.ahdr.application == serviceId)
					service.ack();
//				else
//					channel.reset();
			}
//			Serial.println("# !");
///////////			send();
		}
	}


	template<class T>
	inline void service_tx(uint8_t serviceId, T&service) {
		krf.packet.ahdr.application=serviceId;
		if (connected()) {
			if (service.takeInitiative()) {
				send();
			} else {
			}
		} else {
			connect();
		}
	}

};
