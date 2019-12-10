/*
 * Pilot.h
 *
 *  Created on: Feb 25, 2013
 *      Author: kirk
 */

#ifndef PILOT_H_
#define PILOT_H_

#include "AFMotor.h"

class Pilot {
public:
	Pilot();
	void	drive(int dl,int dr);
	void	direction(int dl,int dr);
	void	throttle(int tl,int tr);
	virtual ~Pilot();
};

#endif /* PILOT_H_ */
