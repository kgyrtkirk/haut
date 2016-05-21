#pragma	once
#include "RF24.h"

struct	KRFState {
	uint16_t	pir:1;
	uint16_t	lum:15;
	float		temp;
	float		hum;
	uint8_t		strip_bright;
};

//#define	KRF_KITCHEN	"ksense"
#define	KRF_KITCHEN_STRIP	"strip0"
#define	KRF_KITCHEN			"2Node"
#define	KRF_DESK0			"desk00"

class KRF{
	RF24		radio;
	const char* myAddr;
public:
	KRFState	state;

	KRF(uint8_t cepin,uint8_t cspin,const char*_myAddr) : radio(cepin,cspin) {
		myAddr=_myAddr;
	}
	void	listenTo(uint8_t idx,const char*_myAddr);
	void 	begin();
	void	send();
	void 	listen(uint32_t timeout);
	void	debug();
};
