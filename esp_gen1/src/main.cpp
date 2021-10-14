#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <stdlib.h>
#include <Wire.h>

#include "k-settings.h"
#include "k-espcore.h"

extern ESP8266WiFiMulti WiFiMulti;
extern WiFiClient espClient;
extern PubSubClient client;
long lastMsg = 0;
char msg[128];
char channel[128];
int value = 0;
extern const char*devicePrefix ;

struct PromValues {
        float   humidity=3;
		float	temperature=33;
		unsigned long uptime=0;
		unsigned long commDelay=0;

        String getValues() {
		    String s;
			#define V(K)	s+="haut_" #K " " + String(K) + String("\n");
			V(humidity);
			V(temperature);
			V(uptime);
			V(commDelay);
			#undef V
			return s;
        }
} promValues;

bool bathroom=false;
#define MAC_BATHROOM	"60:01:94:10:16:AE"
#define MAC_KITCHEN		"60:01:94:0F:CE:44"
void setup_wifi0() {

	delay(10);
	Serial.println();
	Serial.println("Startup...");
	Serial.print("connecting to ");
	Serial.println(WIFI_SSID);
	WiFi.mode(WIFI_STA);
	WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);


	while (WiFiMulti.run() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	randomSeed(micros());

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	
}

uint64_t manualUntil = 0;
int		lampManualVal;
#define	MANUAL_TIME_S	600

int last_lamp_val = -1;

void setLamp(int val, bool force) {
//	if (last_lamp_val == val && !force)
//		return;
	last_lamp_val = val;
	Wire.beginTransmission(0x33);
	Wire.write(val);
	Wire.write(16);
	Wire.write(16);
	Wire.endTransmission();
}

#include "DHT.h"
#define DHTPIN 16
//#define DHTPIN 5
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

#define IR_RECV_PIN 4
#define HALL_PIN 14

#include "DelayControlValue.h"
DelayControlValue<uint8_t, 8> lampCtrl;

KWebServer webServer;

// FIXME: how to use ref to class method?
String getMetricsValues1() {
//	return "asd";
	return promValues.getValues();
}

KMqttClient     kmqtt;

int parseInt(byte* payload, unsigned int length) {
	if(length>=10) {
		return 128;
	}
	char    tmp[10];
	strncpy(tmp,(char*)payload,length);
	tmp[length]=0;

	return atoi(tmp);
}

void lampCallback(char* topic, byte* payload, unsigned int length) {
	manualUntil = millis() + MANUAL_TIME_S * 1000;
	int val = parseInt(payload,length);
	
	char msg[128];
	sprintf(msg, "analog: %d", val);
	client.publish("outTopic", msg);

	lampManualVal=val;
	setLamp(val, true);
}

int maxLight=255;

void maxLightCallback(char* topic, byte* payload, unsigned int length) { 
	int val = parseInt(payload,length);
	if(val>0 && val<=255)
		maxLight=val;
}

bool hasStrip=false;
#include <KStrip.h>
KStrip kstrip;

void setup() {

	const char*devicePrefix = "unknown";

	if (WiFi.macAddress() == MAC_BATHROOM) {
		devicePrefix = "bathroom";
		bathroom=true;
	}
	if (WiFi.macAddress() == MAC_KITCHEN) {
		devicePrefix = "kitchen";
	}
	if (WiFi.macAddress() == "60:01:94:0F:67:7F") {
		devicePrefix = "g3";
	}
	if (WiFi.macAddress() == "D8:F1:5B:02:5D:2B") {
		devicePrefix = "wide-dev";
		hasStrip = true;
	}

	Serial.begin(115200);

	lampCtrl.command(0, 0, 0);
	pinMode(A0, INPUT);
	pinMode(16, INPUT);
	pinMode(4, OUTPUT);
	pinMode(HALL_PIN, INPUT);
	pinMode(IR_RECV_PIN, INPUT);
	pinMode(BUILTIN_LED, OUTPUT);
	Wire.pins(12, 13);
	Wire.begin();

	dht.begin();

	kmqtt.init(devicePrefix);
	kmqtt.subscribe("lamp", lampCallback);

	kmqtt.subscribe("maxLight", maxLightCallback);

//	setup_wifi();
	client.setServer(MQTT_SERVER, 1883);
//	client.setCallback(callback);

	webServer.init(getMetricsValues1);

	if(hasStrip) {
		kstrip.init();
	}

}
//#define LIGHT_DEBUG
#ifdef LIGHT_DEBUG
#define	LAMP_ON_1_TIME_MS		3*1000
#define	LAMP_ON_2_TIME_MS		6*1000
#define	LAMP_ON_3_TIME_MS		20*1000
#else
#define	LAMP_ON_1_TIME_MS		60*1000
#define	LAMP_ON_2_TIME_MS		90*1000
#define	LAMP_ON_3_TIME_MS		300*1000
#endif

bool isHumidityHigh(){
	float hum = dht.readHumidity();
	return (55.0f <= hum && hum <= 101.0f);
}

void loop() {
	kmqtt.loop();

	long now = millis();
	bool secPassed = now - lastMsg > 1000;
	uint64_t t0=micros64();
	if (secPassed && client.connected()) {
		lastMsg = now;
		++value;
		int hum = 0;
		int temp = 0;

		hum = (dht.readHumidity() * 100);
		temp = (dht.readTemperature() * 100);
		promValues.humidity=dht.readHumidity();
		promValues.temperature=dht.readTemperature();
		promValues.uptime= millis();
//    hum=3;
		int lum = analogRead(A0);
//    analogRead(0);
		int pir = digitalRead(5);
		int hall = digitalRead(HALL_PIN);


//    int lum=1;int pir=3;int hall=2;

		int irv = 0;

	/*	sprintf(msg, "%ld", now);
		sprintf(channel, "%s/uptime", devicePrefix);
		client.publish(channel, msg);

		sprintf(msg, "%d.%d", hum / 100, hum % 100);
		sprintf(channel, "%s/humidity", devicePrefix);
		client.publish(channel, msg);

		sprintf(msg, "%d.%d", temp / 100, temp % 100);
		sprintf(channel, "%s/temperature", devicePrefix);
		client.publish(channel, msg);

		sprintf(msg, "%d", lum);
		sprintf(channel, "%s/lum", devicePrefix);
		client.publish(channel, msg);
*/
		sprintf(msg, "eadingsX #%d temp:%d hum:%d lum:%d pir:%d irv:%08x h:%d HH:%d",
				value, temp, hum, lum, pir, irv, hall,isHumidityHigh());

		Serial.print(devicePrefix);
		Serial.print(" @ ");
		Serial.print(WiFi.macAddress());
		Serial.print(":");

		Serial.print("Publish message: ");
		Serial.println(msg);
		client.publish("outTopic", msg);
//		sprintf(msg, "%d", pir);
//		sprintf(channel, "%s/pir", devicePrefix);
//		client.publish(channel, msg);
		if (bathroom && isHumidityHigh()) {
			client.publish("sonoff/run", "600");
		}
	}
	promValues.commDelay+=micros64() - t0;

	delay(10);
	{

		if (now > manualUntil) {
			int pir = digitalRead(5);
			bool highHumidity=isHumidityHigh();
			bool force=secPassed;
			if (pir || highHumidity) {
				force |= lampCtrl.getActiveValue() != 255;
				lampCtrl.command(3, now + LAMP_ON_1_TIME_MS, maxLight);
				lampCtrl.command(2, now + LAMP_ON_2_TIME_MS, maxLight/4*3);
				lampCtrl.command(1, now + LAMP_ON_3_TIME_MS, 1);
			}
			setLamp(lampCtrl.getActiveValue(), force);
		} else {
			setLamp(lampManualVal, secPassed);
		}
	}
	if(hasStrip) {
		Serial.println("strip/update");
		kstrip.update();
	}

}


