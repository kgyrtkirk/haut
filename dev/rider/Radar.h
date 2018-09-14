#ifndef	RADAR_H
#define	RADAR_H

#include "Servo.h"
#include "sonar.h"

enum	RadarDirection{
	LEFT,
	HEAD,
	RIGHT,
	RADAR_DIR_MAX
};

class Radar{
	int		dir_i;
	int		distance[RADAR_DIR_MAX];
	int		error[RADAR_DIR_MAX];
	Servo	servo;
	Sonar&	sonar;

	void nextPosition();
public:
	Radar(Sonar&_sonar);
	void	attach(int pin);
	void	sweep();
	int		range(RadarDirection dir);
	int		getDirection();
	void	debug();
};


#endif
