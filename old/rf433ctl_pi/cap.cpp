//#include <wiringSerial.h>
//#include <Arduino.h>
#include <wiringPi.h>
#include "rf433ctl.h"
#include <stdlib.h>
#include <stdio.h>


#define OUT 8
#define IN 9
#define K	2

void loop();



struct Capture {
	char	value;
	long	t;
	int		error;


	void	doCapture(){
		value=digitalRead(IN);
		t=micros();
	}
};


#define	CAP_LEN	30000

Capture last;
Capture	capArr[CAP_LEN];
int		capOff=0;
long	nLoop=0;

int main() {
  wiringPiSetup () ;

  pinMode(IN, INPUT);

	printf("start\n");
long	tStart=micros();
	while(CAP_LEN!=capOff){
	loop();
	}

long	tEnd=micros();
long tRun=tEnd-tStart;
	printf("tRun:	%ld\n",tRun);
	printf("nLoop:	%ld\n",nLoop);
	printf("aLoop:	%f\n",((float)tRun)/nLoop);

	FILE	*f=fopen("capture.csv","w");
	for(int i=0;i<capOff;i++){
		Capture c=	capArr[i];
		fprintf(f,"%d,%ld,%d\n",c.value,c.t,c.error);
	}
	fclose(f);

}


void loop() {
	nLoop++;
	Capture	current;
	current.doCapture();

	if(last.value!=current.value) {
		current.error=current.t-last.t;
		capArr[capOff]=current;
		capOff++;
	}
	last=current;
}

