/*
 * k-espcore.cpp
 *
 *  Created on: Jul 8, 2019
 *      Author: kirk
 */

#include <Arduino.h>
//#include <ESP8266WebServer.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <k-settings.h>
#include <k-espcore.h>
#include <string>

ESP8266WiFiMulti WiFiMulti;
WiFiClient espClient;
PubSubClient client(espClient);

struct Reg {
	Reg(const char*_t,T_CALL _c) : topic(_t),callback(_c) {};
	const char*topic;
	T_CALL callback;
};
std::vector<Reg> regs;

//#include <WebServer.h>
//ESP8266WebServer server(80);

const char*devicePrefix = "unknown";

void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");

	for (int i = 0; i < length; i++) {
		Serial.print((char) payload[i]);
	}
	Serial.println();


	for(int i=0;i<regs.size();i++){
		if(!strcmp(topic,regs[i].topic)){
			Serial.println("dispatching to callback");
			regs[i].callback(topic,payload,length);
			return;
		}
	}

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
		client.publish(devicePrefix, "online");
		// ... and resubscribe
		char	tmp[128];
		sprintf(tmp,"%s/control",devicePrefix);
		client.subscribe(tmp);
		for(auto i=regs.begin(); i!=regs.end();i++) {
			client.subscribe(i->topic);
		}
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

void setup_wifi();

void KMqttClient::init(const char*_devicePrefix) {
	setup_wifi();
	client.setServer(MQTT_SERVER, 1883);
	client.setCallback(callback);
	devicePrefix=_devicePrefix;
}

void KMqttClient::loop() {
	if (!client.connected()) {
		reconnect();
	} else {
		client.loop();
	}
}
void KMqttClient::subscribe(const char*topic,T_CALL callback) {
	regs.push_back(Reg(topic,callback));
}

void	KMqttClient::publishMetric(std::string topic,long value) {
	if(client.connected()) {
		auto newTopic=topic.replace(topic.find(std::string("@")), 1, std::string(devicePrefix));

		// FIXME: why don't we have std::to_string(value) ?
		char s[128];
		sprintf(s,"%ld",value);
		client.publish(newTopic.c_str(), s);
	}
}
void xbl() ;
extern "C" {
#include "user_interface.h"
}
void setup_wifi() {

	delay(10);
	Serial.println();
	Serial.println("Startup...");
	Serial.print("connecting to ");
	Serial.println(WIFI_SSID);
	wifi_set_phy_mode(PHY_MODE_11G);
	system_phy_set_max_tpw(40);
	WiFi.mode(WIFI_STA);
	WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
	xbl();
	while (WiFiMulti.run() != WL_CONNECTED) {
		xbl();
		Serial.print(".");
	}

	randomSeed(micros());

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}



void xb(int BLINK_DURATION) {
	int LED_SONOFF = 2;
	digitalWrite(LED_SONOFF, LOW); // LOW will turn on the LED
	delay(BLINK_DURATION);
	digitalWrite(LED_SONOFF, HIGH); // HIGH will turn off the LED
	delay(BLINK_DURATION);

}

void xbl() {
	int k=1000;
	int j=50;
	xb(j);k-=j+j;
	xb(j);k-=j+j;
	xb(j);k-=j+j;
	delay(k);
}



void KMqttClient::blink(){
	xbl();
}
