#include "LightMeter.h"

LightMeter::LightMeter(int _adChannel) : adChannel(_adChannel){

}

double LightMeter::light(){
	int v = analogRead(0);
	int	RawADC0=1023 - v;

	double Vout = RawADC0 * 0.0048828125;
//int lux=500/(10*((5-Vout)/Vout));//use this equation if the LDR is in the upper part of the divider
	int lux = (2500 / Vout - 500) / 11;
	return lux;

}

