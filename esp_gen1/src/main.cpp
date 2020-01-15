#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <stdlib.h>
#include <Wire.h>

#include "k-settings.h"

ESP8266WiFiMulti WiFiMulti;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[128];
char channel[128];
int value = 0;
const char*devicePrefix = "unknown";

void setup1();

struct PromValues {
        float     humidity=3;
        float temperature=33;
		unsigned long uptime=0;

        String getValues() {
		    String s;
			#define V(K)	s+="haut_" #K " " + String(K) + String("\n");
			V(humidity);
			V(temperature);
			V(uptime);
			#undef V
			s+="xhaut_humidity "+String(humidity)+String("\n");
			s+="xhaut_temperature "+String(temperature)+String("\n");
			s+="xhaut_uptime "+String(uptime)+String("\n");
			return s;
        }
} promValues;

bool bathroom=false;
#define MAC_BATHROOM	"60:01:94:10:16:AE"
#define MAC_KITCHEN		"60:01:94:0F:CE:44"
void setup_wifi() {
	if (WiFi.macAddress() == MAC_BATHROOM) {
		devicePrefix = "bathroom";
		bathroom=true;
	}
	if (WiFi.macAddress() == MAC_KITCHEN) {
		devicePrefix = "kitchen";
	}

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
	if (last_lamp_val == val && !force)
		return;
	last_lamp_val = val;
	Wire.beginTransmission(0x33);
	Wire.write(val);
	Wire.write(16);
	Wire.write(16);
	Wire.endTransmission();
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
	if ((char) payload[0] == 'A') {
		manualUntil = millis() + MANUAL_TIME_S * 1000;
		int val = strtol(((char*) payload) + 1, 0, 10);

		char msg[128];
		sprintf(msg, "analog: %d", val);
		client.publish("outTopic", msg);

		lampManualVal=val;
		setLamp(val, true);

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
		client.publish("outTopic", "XH3ll0 WorldPIO");
		// ... and resubscribe
		client.subscribe("inTopic");
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

void setup() {
	lampCtrl.command(0, 0, 0);
	pinMode(A0, INPUT);

	pinMode(16, INPUT);     // Initialize the BUILTIN_LED pin as an output

	pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
	Serial.begin(115200);
	setup_wifi();
	client.setServer(MQTT_SERVER, 1883);
	client.setCallback(callback);
	dht.begin();

	pinMode(4, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

	Wire.pins(12, 13);
	Wire.begin();

	pinMode(HALL_PIN, INPUT);
	pinMode(IR_RECV_PIN, INPUT);

	setup1();

}

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
	return (65.0f <= hum && hum <= 101.0f);
}

void loop() {
//	if(WiFiMulti.run() == WL_CONNECTED) {
//		  if (!client.connected()) {
//		    reconnectTry();
//		  } else {
//
//		  }
//	}
	if (!client.connected()) {
		reconnect();
	}

	client.loop();

	long now = millis();
	bool secPassed = now - lastMsg > 1000;
	if (secPassed) {
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

		sprintf(msg, "%ld", now);
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

		sprintf(msg, "eadings #%d temp:%d hum:%d lum:%d pir:%d irv:%08x h:%d HH:%d",
				value, temp, hum, lum, pir, irv, hall,isHumidityHigh());
		Serial.print("Publish message: ");
		Serial.println(msg);
		client.publish("outTopic", msg);
		sprintf(msg, "%d", pir);
		sprintf(channel, "%s/pir", devicePrefix);
		client.publish(channel, msg);
		if (bathroom && isHumidityHigh()) {
			client.publish("sonoff/run", "600");
		}
	}
	delay(10);
	{

		if (now > manualUntil) {
			int pir = digitalRead(5);
			bool highHumidity=isHumidityHigh();
			bool force=secPassed;
			if (pir || highHumidity) {
				force |= lampCtrl.getActiveValue() != 255;
				lampCtrl.command(3, now + LAMP_ON_1_TIME_MS, 255);
				lampCtrl.command(2, now + LAMP_ON_2_TIME_MS, 192);
				lampCtrl.command(1, now + LAMP_ON_3_TIME_MS, 1);
			}
			setLamp(lampCtrl.getActiveValue(), force);
		} else {
			setLamp(lampManualVal, secPassed);
		}
	}
}






#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup1() {

    // Serial.begin(115200);
    // WiFi.mode(WIFI_STA);
    // WiFi.begin(ssid, password);
    // if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //     Serial.printf("WiFi Failed!\n");
    //     return;
    // }

    // Serial.print("IP Address: ");
    // Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "try /metrics");
    });

    server.on("/metrics", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", promValues.getValues());
    });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);

    server.begin();
}

void loop1() {
}