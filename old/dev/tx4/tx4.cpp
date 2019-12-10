// Do not remove the include below
#include "tx4.h"
#include "Pilot.h"


Pilot	pilot;

//Servo	s;
//The setup function is called once at startup of the sketch
void setup()
{
	pinMode(13,OUTPUT);
}

// The loop function is called in an endless loop
int	idx=0;
void loop()
{

	digitalWrite(13,1);
	delay(100);
	digitalWrite(13,0);
	delay(100);
	digitalWrite(13,1);
	delay(100);
	digitalWrite(13,0);
	delay(500);
	idx++;
	if(idx==1){
//		pilot.test();
		pilot.drive(255,255);
	}
	if(idx==20){
		pilot.drive(0,0);
	}
//	delay(5000);
//	pilot.direction(RELEASE);

//	pilot.direction(FORWARD);
//	pilot.throttle(255);

}
