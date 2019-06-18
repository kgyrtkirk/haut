#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <stdlib.h>
#include <Wire.h>
#include <IRremoteESP8266.h>

#include "k-settings.h"

ESP8266WiFiMulti WiFiMulti;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[128];
int value = 0;

void setup_wifi() {

	delay(10);
	// We start by connecting to a WiFi network
	Serial.println();
	Serial.println("Welcome to the disclaimerZ9A! ");
	Serial.print("Connecting to ");
	Serial.println(WIFI_SSID);
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

#define PWM_CHANNELS 1
const uint32_t period = 5000; // * 200ns ^= 1 kHz

// PWM setup
uint32 io_info[PWM_CHANNELS][3] = {
// MUX, FUNC, PIN
		{ PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4, 4 }, };

// initial duty: all off
uint32 pwm_duty_init[PWM_CHANNELS] = { 1 };

IRsend irsend(15);

uint64_t manualUntil = 0;
#define	MANUAL_TIME_S	600

void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++) {
		Serial.print((char) payload[i]);
	}
	Serial.println();

	if ((char) payload[0] == 'R') {
		ESP.restart();
	}
	if ((char) payload[0] == 'U') {
		Serial.println("attempting upgrade");
		t_httpUpdate_return ret = ESPhttpUpdate.update(
				"http://192.168.128.70:8181/update2.php", "wow");
		//t_httpUpdate_return  ret = ESPhttpUpdate.update("https://server/file.bin");

		switch (ret) {
		case HTTP_UPDATE_FAILED:
			Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n",
					ESPhttpUpdate.getLastError(),
					ESPhttpUpdate.getLastErrorString().c_str());
			break;

		case HTTP_UPDATE_NO_UPDATES:
			Serial.println("HTTP_UPDATE_NO_UPDATES");
			break;

		case HTTP_UPDATE_OK:
			Serial.println("HTTP_UPDATE_OK");
			break;
		}
	}
	if ((char) payload[0] == 'W') {
		irsend.sendNEC(0x20DFC03F, 32);

		char msg[128];
		sprintf(msg, "irsend");
		client.publish("outTopic", msg);

	}
}

void reconnectTry() {
	Serial.print("Attempting MQTT connection...");
	String clientId = "ESP8266Client-";
	clientId += String(random(0xffff), HEX);
	if (client.connect(clientId.c_str())) {
		Serial.println("connected");
		// Once connected, publish an announcement...
		client.publish("outTopic", "XH3ll0 World");
		// ... and resubscribe
		client.subscribe("sonoff/command");
	} else {
		Serial.print("failed, rc=");
		Serial.print(client.state());
		Serial.println("wait .5 seconds");
		delay(500);
	}
}

void reconnect() {
	// Loop until we're reconnected
	while (!client.connected()) {
		reconnectTry();
	}
}

#define LED_SONOFF 13

void setup() {
	pinMode(LED_SONOFF, OUTPUT); // Initialize the BUILTIN_LED pin as an output
	Serial.begin(115200);
	setup_wifi();
	client.setServer(MQTT_SERVER, 1883);
	client.setCallback(callback);
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
	bl();
	if (!client.connected()) {
		reconnect();
	}

	client.loop();

	long now = millis();
	if (now - lastMsg > 1000) {

		int state = now;
		sprintf(msg, "S%d", state);
		client.publish("sonoff/state", msg);

	}
	delay(10);

}

