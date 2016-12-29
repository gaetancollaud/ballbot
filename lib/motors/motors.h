#ifndef _MOTORS_H
#define _MOTORS_H

#include <types.h>
#include "single-motor.h"

#define DEBUG_MOTORS

#ifdef DEBUG_MOTORS
	#define MOTORS_DEBUG(...) Serial.print(__VA_ARGS__);
	#define MOTORS_DEBUGLN(...) Serial.println(__VA_ARGS__);
	#define MOTORS_DEBUGVAL(...) Serial.print(__VA_ARGS__);Serial.print("\t");
#else
	#define MOTORS_DEBUG(...)
	#define MOTORS_DEBUGLN(...)
	#define MOTORS_DEBUGVAL(...)
#endif


class Motors{
public:
	Motors(structMotorConfig[], structPid* pid);
	void init();
	void reset();
	void loop(unsigned long, double);

private:
	SingleMotor* motors[];

};

#endif
