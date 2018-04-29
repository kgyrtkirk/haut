#pragma once

#include "Arduino.h"
#include "k-settings.h"

#define	SHUTTER_CMD_MASK	((1<<6)-1)
#define	SHUTTER_CMD_UP		0b011001
#define	SHUTTER_CMD_STOP	0b101010
#define	SHUTTER_CMD_DOWN	0b001000

class RF433Ctl {
	void transmitX(uint64_t cmd);
	void transmit_signal(uint16_t t_one_ms, uint16_t t_zero_ms);
	const int txPin;
public:
	RF433Ctl(int _txPin) :
			txPin(_txPin) {
		pinMode(txPin, OUTPUT);
		digitalWrite(txPin, 0);
	}
	void cmd(uint8_t id,int8_t direction);
};

