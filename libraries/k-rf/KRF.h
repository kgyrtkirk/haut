#pragma	once
#include "RF24.h"
#include "SeqHandler.h"

struct	KRFState {
	uint16_t	pir:1;
	uint16_t	lum:15;
	float		temp;
	float		hum;
	uint8_t		strip_bright;
};

//#define	KRF_KITCHEN	"ksense"
//#define	KRF_KITCHEN_STRIP	"strip0"
//#define	KRF_KITCHEN			"2Node"
//#define	KRF_DESK0			"desk00"

enum KRF_ADDR {
	DESK0=0xa01,
	KITCHEN_STRIP,
	KITCHEN,
};

#define PACKED __attribute__((packed))

class KRF{
	RF24		radio;
	uint32_t	myAddr;
public:

	struct PacketHeader {
		uint16_t	source;
		uint32_t	seqId;
	} PACKED;
	struct Packet_Fw{
		struct PacketHeader _hdr;
		uint8_t				application;
		uint8_t				opcode;
		uint8_t				page;
		uint8_t				offset;
		uint8_t				content[16];
	} PACKED;

	union Packet {
		struct PacketHeader hdr;
		struct Packet_Fw	fw;
		KRFState			state;
	};

//	KRFState	state;
//	static
	Packet		packet;

	KRF(uint8_t cepin,uint8_t cspin,const uint32_t _myAddr) : radio(cepin,cspin) {
		myAddr=_myAddr;
	}
	void	listenTo(uint8_t idx,const uint32_t _myAddr);
	void 	begin();
	void	send();
	bool	listen(uint16_t timeout);
	void	debug();
	void sendTo(const uint32_t dest,const char*packet,uint8_t len);
};
