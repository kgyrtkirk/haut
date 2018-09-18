#include "Radar.h"
#include "Arduino.h"
#include	"sonar.h"

const	uint8_t radar_servo_offset[RADAR_DIR_MAX]={120,90,60};

Radar::Radar(Sonar&_sonar) : sonar(_sonar){
}

void	Radar::attach(int pin){
	servo.attach(pin);
}

#define	K	3
void	Radar::sweep(){
	int	dir=getDirection();
	int	m[K];
	int	s=0;
	for(int i=0;i<K;i++){
		m[i]=sonar.measure();
		s+=m[i];
		delay(2);
	}
	s/=K;
	distance[dir]=s;
	int	s2=0;
	for(int i=0;i<K;i++){
		s2+=(m[i]-s)*(m[i]-s);
	}

	error[dir]=s2/K;
	nextPosition();
}

void	Radar::nextPosition(){
	dir_i++;
	int	dir=getDirection();
	servo.write(radar_servo_offset[dir]);
}
int		Radar::getDirection(){
	int	s=dir_i%(2*RADAR_DIR_MAX-2);
	if(s>=RADAR_DIR_MAX){
		s-=RADAR_DIR_MAX;
		s=(RADAR_DIR_MAX-2)-s;
	}
	return	s;
}

int		Radar::range(RadarDirection dir){
	return	distance[dir];
}

void	Radar::debug(){
	printf("dists:	%d[%d]	%d[%d]	%d[%d]\r\n",distance[LEFT],error[LEFT]
			,distance[HEAD],error[HEAD]
			,distance[RIGHT],error[RIGHT]
			);
}
