#pragma once
#include "DelayControlValue.h"
#include "Arduino.h"

struct	FaderTargetValue {
	uint8_t	target;
	uint8_t	skipCount;
};


template<size_t K>
class Fader {
	uint8_t	value;
	uint8_t	skips;
	const uint8_t	ledPin;
public:
	DelayControlValue<FaderTargetValue,K>			dcv;
//	uint8_t	target;
//	uint8_t	skipCount;

	Fader(const uint8_t _ledPin) : ledPin(_ledPin) {
		value=0;
		skips=0;
	}
	void init() {
		TCCR2B = (4<<CS20);
		TIMSK2 = 1<<TOIE2;
	}
	void isr(){
		if(skips) {
			skips--;
			return;
		}
		FaderTargetValue v=dcv.getActiveValue();
//		skipCount=v.skipCount;
//		target=v.target;

		skips=v.skipCount;
		if(value==v.target){
			return;
		}
		if(value<v.target){
			value++;
		}else{
			value--;
		}
		analogWrite(ledPin, value);
	}
};

