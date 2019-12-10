#pragma once
#include "stdint.h"
#include "Arduino.h"

class LightMeter {
	uint8_t adChannel;

public:
	LightMeter(uint8_t _adChannel);
	/* returns luminous flux estimation in lux */
	double	light();
};



class LightMeter2 {
	const uint8_t adChannel;

public:
	LightMeter2(uint8_t _adChannel) : adChannel(_adChannel){
	}
	/* returns luminous flux estimation in lux */
	double	light() {
		int v = analogRead(adChannel);
		int	RawADC0=1023 - v;

		double Vout = RawADC0 * 0.0048828125;
	int lux=500/(11*((5-Vout)/Vout));//use this equation if the LDR is in the upper part of the divider
//		int lux = (2500 / Vout - 500) / 11;
		return lux;
	}
};

