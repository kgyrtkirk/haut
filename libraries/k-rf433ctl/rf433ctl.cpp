#include <Arduino.h>
#include "rf433ctl.h"

uint64_t shutter_ids[]={ SHUTTER_IDS };

#define	TX_SIGNAL_H			703,	10074
#define	TX_SIGNAL_I			4781,	1522
#define	TX_SIGNAL_0			345,	739
#define	TX_SIGNAL_1			704,	378

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

	for(int i=0;i<18;i++) {
		transmit_signal(TX_SIGNAL_0);
		transmit_signal(TX_SIGNAL_1);
	}
	transmitX(msg);
	transmitX(msg);
	transmitX(msg);
	transmitX(msg);
	transmitX(msg);
}

void RF433Ctl::transmitX(uint64_t msg) {
	transmit_signal(TX_SIGNAL_H);
	transmit_signal(TX_SIGNAL_I);

	uint64_t m = 1;
	m<<=38;

	while (m > 0ll) {
		uint8_t bit = (msg & m) ? 1 : 0;
		if (bit) {
			transmit_signal(TX_SIGNAL_1);
		} else {
			transmit_signal(TX_SIGNAL_0);
		}
		m >>= 1;
	}
	transmit_signal(TX_SIGNAL_H);
	transmit_signal(TX_SIGNAL_I);
//	transmit_signal(TX_SIGNAL_H);
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
