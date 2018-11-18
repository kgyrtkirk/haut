#include <wiringPi.h>
#include "rf433ctl.h"

uint64_t shutter_ids[]={ SHUTTER_IDS };

void RF433Ctl::cmd(uint8_t id,int8_t direction){
	if (id >= sizeof(shutter_ids) / sizeof(long)) {
		return;
	}
	uint64_t	msg=shutter_ids[id];
	msg|=SHUTTER_CMD_MASK;
	msg^=SHUTTER_CMD_MASK;
	switch(direction){
		case	-1:	msg|=SHUTTER_CMD_UP;	break;
		case	0:	msg|=SHUTTER_CMD_STOP;	break;
		case	1:	msg|=SHUTTER_CMD_DOWN;	break;
		default:
			return;
	}
	//Serial.print("dir:");
//	Serial.println(direction);
	transmitX(msg);
	transmitX(msg);
	transmitX(msg);
	transmitX(msg);
	transmitX(msg);
}

void RF433Ctl::transmitX(uint64_t msg) {
	// 688,10143
	transmit_signal(688, 10143);
	transmit_signal(4769, 1538);

	uint64_t m = 1;
	m<<=38;
//	Serial.print(sizeof(long));
//	Serial.println("S");
//	Serial.print(m==0);
//	Serial.println("H");
	while (m > 0ll) {
		uint8_t bit = (msg & m) ? 1 : 0;
		if (bit) {
			transmit_signal(687, 394);
		} else {
			transmit_signal(328, 756);
		}
		m >>= 1;
	}
	transmit_signal(688, 10143);
//	Serial.println("x");
}

void	RF433Ctl::transmit_signal(uint16_t t_one_ms,uint16_t t_zero_ms) {
//	Serial.print(t_one_ms);Serial.print(",");
//			Serial.print(t_zero_ms);Serial.print(",");
	unsigned long t0=micros();
	digitalWrite(txPin,1);
	unsigned long t1=t0+t_one_ms;
	unsigned long t2=t1+t_zero_ms;
	while(micros() < t1);
//	delayMicroseconds(t_one_ms);
	digitalWrite(txPin,0);
	while(micros() < t2);
//	delayMicroseconds(t_zero_ms);
}
