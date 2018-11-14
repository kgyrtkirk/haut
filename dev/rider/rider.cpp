// Do not remove the include below
#include "rider.h"
#include "stdio.h"


#include "Pilot.h"
#include "sonar.h"
#include "Radar.h"
#include "uart.h"

//#include "TimerOne.h"


Pilot	pilot;
//UHSonar	sonar(1);
LoxSonar	sonar(1);
Radar	radar(sonar);

void	scan();

void setup()
{
	uart_init();

	sonar.init();
	pinMode(13,OUTPUT);
	digitalWrite(13,1);
	radar.attach(9);
//	Timer1.initialize(100000);

//	Timer1.attachInterrupt(scan);
//	shutdownTimer.start();
}

// The loop function is called in an endless loop
int	run=0;
int	distance;
int	th=0;
#define	T_START	5000
#define	T_END	65000

void loop1()
{
//	radar.sweep();
	delay(100);
//    Serial.println("hello");
	radar.debug();
	pilot.drive(0,0);


}

#define D_MIN 30
long	radarTime=0;
void loop()
{
	long now=millis();
	int t;

	run= T_START<now && now<T_END;
	if(!run && now>T_END)
	{
		printf(" stopped	%ld\r\n",now);
	    pilot.drive(0,0);
	    return;
	}

	bool radarStep=radarTime<now;

	if(radarStep) {
		radarTime=now+80;
		radar.sweep();
	}

	bool	nearL=radar.range(LEFT)<D_MIN;
	bool	nearR=radar.range(RIGHT)<D_MIN;
	bool	nearH=radar.range(HEAD)<D_MIN;

	if(radarStep)
		printf(" run	%ld	R:%d,%d,%d\r\n",now,radar.range(LEFT),radar.range(HEAD),radar.range(RIGHT));

	if(!run){
	    pilot.drive(0,0);
	    return;
	}

	if(nearL && nearR)
		pilot.drive(-255,255);
	else
	{
		if(nearH){
			if(nearL || nearR){
				if(nearL)
					pilot.drive(255,-255);
				if(nearR)
					pilot.drive(-255,255);
			}else
				pilot.drive(-255,255);
		}else{
			if(nearL || nearR){
				if(nearL)
					pilot.drive(255,200);
				if(nearR)
					pilot.drive(200,255);
			}else
				pilot.drive(255,255);
		}
	}

	delay(5);


}

//void	scan(){
//	idx++;
//	digitalWrite(13,idx%2);
//	if(idx>10*10)
//		run = 1;
//	if(idx>20*10)
//		run = 0;
//
//	distance=sonar.measure();
//}
