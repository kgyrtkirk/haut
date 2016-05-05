#include "LightMeter.h"
#include "Arduino.h"

LightMeter::LightMeter(uint8_t _adChannel) : adChannel(_adChannel){

}

double LightMeter::light(){
	int v = analogRead(adChannel);
	int	RawADC0=1023 - v;

	double Vout = RawADC0 * 0.0048828125;
//int lux=500/(10*((5-Vout)/Vout));//use this equation if the LDR is in the upper part of the divider
	int lux = (2500 / Vout - 500) / 11;
	return lux;

}

