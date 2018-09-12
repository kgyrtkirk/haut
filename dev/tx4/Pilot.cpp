/*
 * Pilot.cpp
 *
 *  Created on: Feb 25, 2013
 *      Author: kirk
 */

#include "Pilot.h"
#include "math.h"

class	AF_DCMotorInv : public AF_DCMotor{
public:
	AF_DCMotorInv(int idx) : AF_DCMotor(idx){
	}

	void	run(int dir){
		switch(dir){
		case	FORWARD:	AF_DCMotor::run(BACKWARD);	break;
		case	BACKWARD:	AF_DCMotor::run(FORWARD);	break;
		default:
			AF_DCMotor::run(dir);
		}
	}
};

	AF_DCMotor fl(2);
	AF_DCMotorInv fr(1);
	AF_DCMotor bl(3);
	AF_DCMotorInv br(4);

Pilot::Pilot() {
	direction(RELEASE,RELEASE);
}

void	Pilot::drive(int dl,int dr){
	int	dir_l=RELEASE;
	if (dl == 0)
		dir_l = RELEASE;
	else {
		dir_l = (dl < 0) ? BACKWARD : FORWARD;
	}
	int	dir_r=RELEASE;
	if (dr == 0)
		dir_r = RELEASE;
	else {
		dir_r = (dr < 0) ? BACKWARD : FORWARD;
	}
	direction(dir_l,dir_r);
	if(dl<0)
		dl=-dl;
	if(dr<0)
		dr=-dr;
	throttle(dl,dr);
}

void	Pilot::direction(int dl,int dr){
	fl.run(dl);
	fr.run(dr);
	bl.run(dl);
	br.run(dr);
}

void	Pilot::throttle(int tl,int tr){
	fl.setSpeed(tl);
	fr.setSpeed(tr);
	bl.setSpeed(tl);
	br.setSpeed(tr);
}


void Pilot::test()
{
	fr.setSpeed(255);
}

Pilot::~Pilot() {
	// TODO Auto-generated destructor stub
}

