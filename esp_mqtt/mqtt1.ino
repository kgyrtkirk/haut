/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <stdlib.h>
#include <Wire.h>
#include <IRremoteESP8266.h>

#include "k-settings.h"

#define FET_PIN	4
// Update these with values suitable for your network.

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
    {PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4 ,  4},
};

// initial duty: all off
uint32 pwm_duty_init[PWM_CHANNELS] = {1};


IRsend irsend(15);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == 'U') {
      Serial.println("attempting upgrade");
      t_httpUpdate_return ret = ESPhttpUpdate.update("http://192.168.128.70:8181/update2.php","wow");
      //t_httpUpdate_return  ret = ESPhttpUpdate.update("https://server/file.bin");

      switch(ret) {
          case HTTP_UPDATE_FAILED:
              Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
              break;

          case HTTP_UPDATE_NO_UPDATES:
        	  Serial.println("HTTP_UPDATE_NO_UPDATES");
              break;

          case HTTP_UPDATE_OK:
        	  Serial.println("HTTP_UPDATE_OK");
              break;
      }
  }
  if ((char)payload[0] == 'A') {
	  int val=strtol(((char*)payload)+1,0,10);
//	  analogWrite(FET_PIN,val);
//	  pwm_set_duty(val, 0); // GPIO15: 100%
//	  pwm_start();           // commit

      char msg[128];
      sprintf (msg, "analog: %d", val);
      client.publish("outTopic", msg);

      Wire.beginTransmission(0x33);
      Wire.write(val);
      Wire.endTransmission();

  }
  if ((char)payload[0] == 'W') {
	           irsend.sendNEC(0x20DFC03F,32);

      char msg[128];
      sprintf (msg, "irsend");
      client.publish("outTopic", msg);

  }
  if ((char)payload[0] == '1') {
//    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
//    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
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
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
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

IRrecv irrecv(IR_RECV_PIN);

void setup() {
	pinMode(A0, INPUT);

	  pinMode(16, INPUT);     // Initialize the BUILTIN_LED pin as an output

  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
  dht.begin();


  pinMode(4, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
irsend.begin();

  Wire.pins(12,13);
  Wire.begin();

  pinMode(HALL_PIN, INPUT);
  pinMode(IR_RECV_PIN, INPUT);
  irrecv.enableIRIn(); // Start the receiver

//  analogWrite(FET_PIN,16);
}

decode_results results;
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    int hum=0;
    int temp=0;
    hum=(dht.readHumidity()*100);
    temp=(dht.readTemperature()*100);
//    hum=3;
    int lum= analogRead(A0);
//    analogRead(0);
    int pir=digitalRead(5);
    int hall=digitalRead(HALL_PIN);
//    int lum=1;int pir=3;int hall=2;

	int irv=0;
	if (irrecv.decode(&results)) {
    irv=(unsigned int)results.value;

    irrecv.resume(); // Receive the next value
	}



	sprintf (msg, "readings #%d temp:%d hum:%d lum:%d pir:%d irv:%08x h:%d", value, temp,hum,lum,pir,irv,hall);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
//    delay(200);

  }
}


