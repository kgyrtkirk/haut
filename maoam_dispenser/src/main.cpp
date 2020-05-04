
#include "k-espcore.h"
#include <CheapStepper.h>

// next, declare the stepper
// and connect pins 8,9,10,11 to IN1,IN2,IN3,IN4 on ULN2003 board

CheapStepper stepper (D5,D6,D7,D8); 


boolean moveClockwise = true;

KMqttClient     kmqtt;

void move(int steps){ 
  Serial.print(" move: ");
  Serial.println(steps);
  if(steps<0) {
    stepper.moveDegrees (true, -steps);
  }else{
    stepper.moveDegrees (false, steps);
  }
  stepper.off();
}

int parseInt(byte* payload, unsigned int length) {
    if(length>=10) {
        return 0;
    }
    char    tmp[10];
    strncpy(tmp,(char*)payload,length);
    tmp[length]=0;
    return atoi(tmp);
}

void moveCall(char* topic, byte* payload, unsigned int length) {
    move(parseInt(payload,length));
}

void dispenseCall(char* topic, byte* payload, unsigned int length) {
  int n=4.5*360/12;
  move(n);
}
void dispenseCall2(char* topic, byte* payload, unsigned int length) {
  int n=4.5*360/12;
  move(-n);
}

int permits=0;
int bbSolved=0;

void permitsCall(char* topic, byte* payload, unsigned int length) {
  permits=parseInt(payload,length);
  bbSolved=0;
}

void winCheck() {
  if( bbSolved && permits>0) {
    bbSolved=0;
    kmqtt.publishMetric(std::string("@/permits"), --permits);
    dispenseCall(0,0,0);
  }
}

void bbCall(char* topic, byte* payload, unsigned int length) {
  bbSolved= (parseInt(payload, length)==1111);
}


void setup() {
  Serial.begin(115200);
  kmqtt.init("maoam");
  

//  kmqtt.subscribe("shutterctl/dispense",&dispense);
  kmqtt.subscribe("maoam/move",&moveCall);
  kmqtt.subscribe("maoam/dispense",&dispenseCall);
  kmqtt.subscribe("maoam/dispense2",&dispenseCall2);
  kmqtt.subscribe("maoam/permits",&permitsCall);
  kmqtt.subscribe("busy_board/state",&bbCall);


  // let's set a custom speed of 20rpm (the default is ~16.25rpm)
  
  stepper.setRpm(15); 
  /* Note: CheapStepper library assumes you are powering your 28BYJ-48 stepper
   * using an external 5V power supply (>100mA) for RPM calculations
   * -- don't try to power the stepper directly from the Arduino
   * 
   * accepted RPM range: 6RPM (may overheat) - 24RPM (may skip)
   * ideal range: 10RPM (safe, high torque) - 22RPM (fast, low torque)
   */

  // now let's set up a serial connection and print some stepper info to the console
  
  Serial.begin(9600); Serial.println();
  Serial.print(stepper.getRpm()); // get the RPM of the stepper
  Serial.print(" rpm = delay of ");
  Serial.print(stepper.getDelay()); // get delay between steps for set RPM
  Serial.print(" microseconds between steps");
  Serial.println();
  
  stepper.off();
  // stepper.setTotalSteps(4076);
  /* you can uncomment the above line if you think your motor
   * is geared 63.68395:1 (measured) rather than 64:1 (advertised)
   * which would make the total steps 4076 (rather than default 4096)
   * for more info see: http://forum.arduino.cc/index.php?topic=71964.15
   */
}

void loop() {
    kmqtt.loop();
    delay(50);  
    winCheck();
}
