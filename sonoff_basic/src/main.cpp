#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <stdlib.h>
//#include <Wire.h>
//#include <IRremoteESP8266.h>

#include "k-settings.h"

// #define S
#ifndef S
#pragma GCC poison Serial
#endif

#define LED_SONOFF	13
#define RELAY		12
#define BUTTON		0


ESP8266WiFiMulti WiFiMulti;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[128];
int value = 0;

void println() {
	#ifdef S
	Serial.println();
	#endif
}

void print(const String&s) {
	#ifdef S
	Serial.print(s);
	#endif
}

void println(const String&s) {
	#ifdef S
	Serial.println(s);
	#endif
}
void println(const Printable&s) {
	#ifdef S
	Serial.println(s);
	#endif
}


void setup_wifi() {

	delay(10);
	// We start by connecting to a WiFi network

	println();
	println("Welcome to the disclaimerZA! ");
	print("Connecting to ");
	println(WIFI_SSID);
	WiFi.mode(WIFI_STA);
	WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

	while (WiFiMulti.run() != WL_CONNECTED) {
		delay(500);
		print(".");
	}

	randomSeed(micros());

	println("");
	println("WiFi connected");
	println("IP address: ");
	println(WiFi.localIP());
}

#define PWM_CHANNELS 1
const uint32_t period = 5000; // * 200ns ^= 1 kHz

// PWM setup
uint32 io_info[PWM_CHANNELS][3] = {
// MUX, FUNC, PIN
		{ PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4, 4 }, };

// initial duty: all off
uint32 pwm_duty_init[PWM_CHANNELS] = { 1 };

//IRsend irsend(15);

uint64_t manualUntil = 0;
#define	MANUAL_TIME_S	600

void callback(char* topic, byte* payload, unsigned int length) {
	print("Message arrived! [");
	print(topic);
	print("] ");
	for (int i = 0; i < length; i++) {
		print(String((char) payload[i]));
	}
	println();

	if (!strcmp(topic, "sonoff/run")) {
		manualUntil = millis() + 600 * 1000;
		return;
	}

	if ((char) payload[0] == 'R') {
		ESP.restart();
	}
	if ((char) payload[0] == 'U') {
		println("attempting upgrade");
		t_httpUpdate_return ret = ESPhttpUpdate.update(
				"http://192.168.128.70:8181/update2.php", "wow");
		//t_httpUpdate_return  ret = ESPhttpUpdate.update("https://server/file.bin");

		switch (ret) {
		case HTTP_UPDATE_FAILED:
		#ifdef S
			Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n",
					ESPhttpUpdate.getLastError(),
					ESPhttpUpdate.getLastErrorString().c_str());
					#endif
			break;

		case HTTP_UPDATE_NO_UPDATES:
			println("HTTP_UPDATE_NO_UPDATES");
			break;

		case HTTP_UPDATE_OK:
			println("HTTP_UPDATE_OK");
			break;
		}
	}
	if ((char) payload[0] == 'W') {
		//irsend.sendNEC(0x20DFC03F, 32);

		char msg[128];
		sprintf(msg, "irsend");
		client.publish("outTopic", msg);

	}
	if ((char) payload[0] == 'X') {
		digitalWrite(RELAY, HIGH);
	}
	if ((char) payload[0] == 'Y') {
		digitalWrite(RELAY, LOW);
	}

}

void reconnectTry() {
	print("Attempting MQTT connection...");
	String clientId = "ESP8266Client-";
	clientId += String(random(0xffff), HEX);
	if (client.connect(clientId.c_str())) {
		println("connected");
		// Once connected, publish an announcement...
		client.publish("sonoff", "XH3ll0 World");
		// ... and resubscribe
		client.subscribe("sonoff/command");
		client.subscribe("sonoff/run");
	} else {
		print("failed, rc=");
		print(String(client.state()));
		println("wait .5 seconds");
		delay(500);
	}
}

void reconnect() {
	// Loop until we're reconnected
	while (!client.connected()) {
		reconnectTry();
	}
}

void setup() {
	pinMode(LED_SONOFF, OUTPUT); // Initialize the BUILTIN_LED pin as an output
	digitalWrite(LED_SONOFF, 0);
	pinMode(RELAY, OUTPUT);
#ifdef S
	Serial.begin(115200);
#endif
	setup_wifi();
	client.setServer(MQTT_SERVER, 1883);
	client.setCallback(callback);
	digitalWrite(RELAY, LOW);
}

void b(int BLINK_DURATION) {
	digitalWrite(LED_SONOFF, LOW); // LOW will turn on the LED
	delay(BLINK_DURATION);
	digitalWrite(LED_SONOFF, HIGH); // HIGH will turn off the LED
	delay(BLINK_DURATION);

}

void bl() {
	int k=1000;
	int j=50;
	b(j);k-=j+j;
	b(j);k-=j+j;
	b(j);k-=j+j;
	delay(k);
}

void loop() {
	if (!client.connected()) {
		bl();
		reconnect();
	}

	client.loop();

	long now = millis();
	if(client.connected()) {
		if (now - lastMsg > 1000) {

			int state = now;
			sprintf(msg, "S%d", state);
			client.publish("sonoff/state", msg);
			lastMsg=now;
		}
	}
		if (now < manualUntil) {
			digitalWrite(RELAY, HIGH);
		} else {
			if (manualUntil != 0) {
				digitalWrite(RELAY, LOW);
				manualUntil = 0;
			}
		}
	delay(10);

}

