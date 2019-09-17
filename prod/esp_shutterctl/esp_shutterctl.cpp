#include "Arduino.h"
#include "esp_shutterctl.h"
#include "rf433ctl.h"
#include "k-espcore.h"

// Pin config
int LED_SONOFF = 2;
int LED_SONOFF2 = 16;

RF433Ctl rfctl(15);
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

#define N_SHUTTER 1000

void addMove(int idx,long delta){
	Serial.printf("addMove idx:%d	delta:%ld\r\n",idx,delta);
	ShutterMove m;
	m.started=0;
	m.idx=idx;
	m.direction=delta>0?1:-1;
	m.moveTime=delta*40000/N_SHUTTER;
	if(m.moveTime<0)
		m.moveTime=-m.moveTime;
	moves.push_back(m);
}

void 	moveShutter(int idx,int targetPos) {
	Serial.printf("moveShutter idx:%d	targetPos:%ld\r\n",idx,targetPos);
	ShutterState &s=shutter[idx];
	if(s.position==targetPos)
		return;
	addMove(idx, targetPos-s.position);
	s.position=targetPos;
	if (s.position < 0)
		s.position = 0;
	if (s.position > N_SHUTTER)
		s.position = N_SHUTTER;
}

void 	moveShutter(int idx, byte* payload, unsigned int length) {
	if(length>=10) {
		return;
	}
	char	tmp[10];
	strncpy(tmp,(char*)payload,length);
	tmp[length]=0;
	moveShutter(idx,atoi(tmp));
}

void c0(char* topic, byte* payload, unsigned int length) {
	moveShutter(0,payload, length);
}
void c1(char* topic, byte* payload, unsigned int length) {
	moveShutter(1,payload, length);
}
void c2(char* topic, byte* payload, unsigned int length) {
	moveShutter(2,payload, length);
}
void c3(char* topic, byte* payload, unsigned int length) {
	moveShutter(3,payload, length);
}
void c4(char* topic, byte* payload, unsigned int length) {
	moveShutter(4,payload, length);
}

void setup() {
	Serial.begin(115200);
	kmqtt.init("shutterctl");
	kmqtt.subscribe("shutterctl/window0",&c0);
	kmqtt.subscribe("shutterctl/window1",&c1);
	kmqtt.subscribe("shutterctl/window2",&c2);
	kmqtt.subscribe("shutterctl/window3",&c3);
	kmqtt.subscribe("shutterctl/window4",&c4);

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
		//Serial.printf("d:%d	t: %ld	started: %ld	moveTime: %ld\r\n",m.direction,tMillis,m.started,m.moveTime);
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




