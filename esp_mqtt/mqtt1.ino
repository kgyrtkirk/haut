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
//extern "C" {
//  #include "pwm.h"
//}

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <stdlib.h>

#define FET_PIN	4
// Update these with values suitable for your network.

const char* ssid = "kroute";
fix_passwd
const char* password = "fixed_later";
const char* mqtt_server = "192.168.128.33";

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
  Serial.println("Welcome to the disclaimer2! ");
  Serial.print("Connecting to ");
  Serial.println(ssid);
fix_passwd
  WiFiMulti.addAP("kroute", "fkfkf");
//  WiFi.begin(ssid, password);

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
uint32 pwm_duty_init[PWM_CHANNELS] = {2500};



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
      t_httpUpdate_return ret = ESPhttpUpdate.update("http://192.168.128.33:8181/update2.php","wow");
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
	  analogWrite(FET_PIN,val);
//	  pwm_set_duty(val, 0); // GPIO15: 100%
//	  pwm_start();           // commit

      char msg[128];
      sprintf (msg, "analog: %d", val);
      client.publish("outTopic", msg);

  }
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
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

volatile int	wheel=0;
static int* wheel1 = (int*)(0x60000300);

extern "C"{
void ICACHE_FLASH_ATTR int_handler(void)
{

	wheel+=1;
//	(*wheel1)++;
}
}



// definitions for RTC Timer1
#define TIMER1_DIVIDE_BY_1              0x0000
#define TIMER1_DIVIDE_BY_16             0x0004
#define TIMER1_DIVIDE_BY_256            0x0008

#define TIMER1_AUTO_LOAD                0x0040
#define TIMER1_ENABLE_TIMER             0x0080
#define TIMER1_FLAGS_MASK               0x00cc

#define TIMER1_NMI                      0x8000

#define TIMER1_COUNT_MASK               0x007fffff        // 23 bit timer

void ICACHE_FLASH_ATTR
timer1Start(uint32_t ticks, uint16_t flags, void (*handler)(void))
{
    RTC_REG_WRITE(FRC1_LOAD_ADDRESS, ticks & TIMER1_COUNT_MASK);
    RTC_REG_WRITE(FRC1_CTRL_ADDRESS, (flags & TIMER1_FLAGS_MASK) | TIMER1_ENABLE_TIMER);
    RTC_CLR_REG_MASK(FRC1_INT_ADDRESS, FRC1_INT_CLR_MASK);
    if (handler != NULL)
    {
//        if (flags & TIMER1_NMI)
//            ETS_FRC_TIMER1_NMI_INTR_ATTACH(handler);
//        else
            ETS_FRC_TIMER1_INTR_ATTACH((void (*)(void *))handler, NULL);
        TM1_EDGE_INT_ENABLE();
        ETS_FRC1_INTR_ENABLE();
        RTC_REG_WRITE(FRC1_LOAD_ADDRESS, ticks & TIMER1_COUNT_MASK);
    }
}

void ICACHE_FLASH_ATTR
timer1Stop(void)
{
    ETS_FRC1_INTR_DISABLE();
    TM1_EDGE_INT_DISABLE();
    RTC_REG_WRITE(FRC1_CTRL_ADDRESS, 0);
}



#include "DHT.h"
#define DHTPIN 5
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

void setup() {
	  pinMode(16, INPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  dht.begin();

//  timer1Start(10000,TIMER1_AUTO_LOAD,int_handler);

//  pwm_init(period, pwm_duty_init, PWM_CHANNELS, io_info);
//  pwm_start();

//  analogWrite(FET_PIN,16);
}

void loop() {

  if (!client.connected()) {
    reconnect();
//    timer1Start(0,0,int_handler);
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    int hum=(dht.readHumidity()*100);
    int temp=(dht.readTemperature()*100);
//    hum=3;
    if(digitalRead(16)>0)
    	sprintf (msg, "XHEllo worlD+#%d %d: %d w%d", value, temp,hum,wheel);
    else
    	sprintf (msg, "Xhello worLd-#%d %d: %d w%d", value, temp,hum,wheel);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
//    delay(200);
  }
}


