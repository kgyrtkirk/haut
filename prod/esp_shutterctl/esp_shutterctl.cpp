#include "Arduino.h"
#include "esp_shutterctl.h"
#include "rf433ctl.h"
#include "k-espcore.h"

// Pin config
int LED_SONOFF = 2;
int LED_SONOFF2 = 16;

RF433Ctl rfctl(3);
KMqttClient	kmqtt;

struct ShutterState {
	int	position;
};
ShutterState	shutter[5];

struct ShutterMove {
	long started;
	int idx;
	int	direction;
	long moveTime;
};
std::vector<ShutterMove> moves;

int	w1=0;
int	w2=0;

// add N_SHUTTER positions

void addMove(int idx,long delta){
	ShutterMove m;
	m.started=0;
	m.idx=idx;
	m.direction=delta>0?1:-1;
	m.moveTime=delta*40000/1000;
	if(m.moveTime<0)
		m.moveTime=-m.moveTime;
	moves.push_back(m);
}

void 	moveShutter(int idx,int targetPos) {
	ShutterState &s=shutter[idx];
	if(s.position==targetPos)
		return;
	addMove(idx, targetPos-s.position);
	s.position=targetPos;
	if (s.position < 0)
		s.position = 0;
	if (s.position > 1000)
		s.position = 1000;
}


void c1(char* topic, byte* payload, unsigned int length) {
	moveShutter(1,atoi((const char*)payload));
}
void c2(char* topic, byte* payload, unsigned int length) {

}

void setup() {
	Serial.begin(115200);
	kmqtt.init("shutterctl");
	kmqtt.subscribe("shutterctl/window1",&c1);

	// initialise digital pin LED_SONOFF as an output.
	pinMode(LED_SONOFF, OUTPUT);

	rfctl.init();
}

void b(int BLINK_DURATION) {
	digitalWrite(LED_SONOFF, LOW); // LOW will turn on the LED
	delay(BLINK_DURATION);
	digitalWrite(LED_SONOFF, HIGH); // HIGH will turn off the LED
	delay(BLINK_DURATION);
}

void processCommands(){
	for(auto it=moves.begin();it!=moves.end();it++) {
		long tMillis=millis();
		auto &m=*it;
		if(!m.started) {
			m.started=tMillis;
			rfctl.cmd(m.idx, m.direction);
		}
		if (tMillis - m.started > m.moveTime) {
			rfctl.cmd(m.idx, 0);
			moves.erase(it--);
		}
	}
}
void loop() {
	kmqtt.loop();

	if(moves.size()>0) {
		for(int i=0;i<100;i++) {
			processCommands();
			delay(10);
		}
	}else{
		b(100);
		b(100);
		delay(600);
	}
}




