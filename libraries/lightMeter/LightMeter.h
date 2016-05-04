#pragma once

class LightMeter {
	int adChannel;

public:
	LightMeter(int _adChannel);
	/* returns luminous flux estimation in lux */
	double	light();
};
