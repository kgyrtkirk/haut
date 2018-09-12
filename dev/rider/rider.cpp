// Do not remove the include below
#include "rider.h"
#include "stdio.h"


#include "Pilot.h"
#include "sonar.h"
#include "Radar.h"
#include "uart.h"

//#include "TimerOne.h"


Pilot	pilot;
Sonar	sonar(1);
Radar	radar(sonar);

void	scan();

void setup()
{
//	Serial.begin(9600);
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
int	idx=0;
int	run=0;
int	distance;
int	th=0;
#define	T_START	5
#define	T_END	65

void loop2()
{
	radar.sweep();
	delay(100);
//    Serial.println("hello");
	radar.debug();
	pilot.drive(0,255);


}

void loop()
{
	idx++;
	int t;
	t = idx / 100;

	run= T_START<t && t<T_END;
	if(!run)
	{
		printf(" stopped	%d\r\n",idx);
	    pilot.drive(0,0);
	    return;
	}

	if(idx%15 == 0)
		radar.sweep();

	bool	nearL=radar.range(LEFT)<30;
	bool	nearR=radar.range(RIGHT)<30;
	bool	nearH=radar.range(HEAD)<30;

	printf(" run	%d	R:%d,%d,%d\r\n",idx,radar.range(LEFT),radar.range(HEAD),radar.range(RIGHT));
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

	delay(10);


}

void	scan(){
	idx++;
	digitalWrite(13,idx%2);
	if(idx>10*10)
		run = 1;
	if(idx>20*10)
		run = 0;

	distance=sonar.measure();
}
