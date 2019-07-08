#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "esp_shutterctl.h"
#include "rf433ctl.h"
#include "Arduino.h"

// Pin config
int LED_SONOFF = 2;
int LED_SONOFF2 = 16;

RF433Ctl rfctl(3);

ESP8266WiFiMulti WiFiMulti;
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() ;

void callback(char* topic, byte* payload, unsigned int length);

void setup() {
	Serial.begin(115200);
	setup_wifi();
	client.setServer(MQTT_SERVER, 1883);
	client.setCallback(callback);

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

void reconnect();

void loop() {
	if (!client.connected()) {
		reconnect();
	}

	client.loop();

	rfctl.cmd(0, -1);
	rfctl.cmd(1, 1);
	rfctl.cmd(2, -1);
	b(100);
	b(100);
	b(100);
	delay(600);
}



const char*devicePrefix = "unknown";

void setup_wifi() {
	devicePrefix = "custom";

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
}

void reconnectTry() {
	// Loop until we're reconnected
	Serial.print("Attempting MQTT connection...");
// Create a random client ID
	String clientId = "ESP8266Client-";
	clientId += String(random(0xffff), HEX);
// Attempt to connect
	if (client.connect(clientId.c_str())) {
		Serial.println("connected");
		// Once connected, publish an announcement...
		client.publish("outTopic", "XH3ll0 World");
		// ... and resubscribe
		client.subscribe("ctrl");
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
