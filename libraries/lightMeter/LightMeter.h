#pragma once
#include "stdint.h"

class LightMeter {
	uint8_t adChannel;

public:
	LightMeter(uint8_t _adChannel);
	/* returns luminous flux estimation in lux */
	double	light();
};
