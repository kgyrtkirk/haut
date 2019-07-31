// Do not remove the include below
#include "esp_gen3.h"


// Pin config
int LED_SONOFF = 2;

KMqttClient	kmqtt;

#include "DHT.h"
DHT dht(16, DHT22);

#include "DelayControlValue.h"
DelayControlValue<uint8_t, 8> lampCtrl;
#include <Wire.h>

int lampVal=0;
void setLamp(int val, bool force);

void 	moveShutter(int idx, byte* payload, unsigned int length) {
	if(length>=10) {
		return;
	}
	char	tmp[10];
	strncpy(tmp,(char*)payload,length);
	tmp[length]=0;

	lampVal=atoi(tmp);
	setLamp(lampVal, true);
}


void c0(char* topic, byte* payload, unsigned int length) {
	moveShutter(0,payload, length);
}

void setup() {
	// initialise digital pin LED_SONOFF as an output.
	pinMode(LED_SONOFF, OUTPUT);
	lampCtrl.command(0, 0, 0);
	Serial.begin(115200);
	kmqtt.init("dev");
	kmqtt.subscribe("dev/light",&c0);


	dht.begin();
	pinMode(A0, INPUT);

	Wire.pins(12, 13);
	Wire.begin();

}

int last_lamp_val = -1;
void setLamp(int val, bool force) {

//	if (last_lamp_val == val && !force)
	//	return;
	last_lamp_val = val;
	Wire.beginTransmission(0x33);
	Wire.write(val);
	Wire.write(16);
	Wire.write(16);
	Wire.endTransmission();
}


void everySecond(){
	int hum;
	int temp;
	hum = (dht.readHumidity() * 100);
	temp = (dht.readTemperature() * 100);
	int lum = analogRead(A0);
//	if (lum < 300)
//	if(millis() % 20000 > 10000)
//		setLamp(255, false);
//	else
	setLamp(lampVal, false);
	int pir = digitalRead(5);
	printf("temp:	%d 	hum:	%d	lum:	%d	pir:	%d\r\n",hum,temp,lum,pir);
}


uint64_t	next_t=0;
void loop() {
	kmqtt.loop();
//	kmqtt.blink();
	uint64_t	now=millis();
	if(now > next_t) {
		next_t=millis()+1000;
		everySecond();
	}
}
