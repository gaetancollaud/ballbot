#ifndef _MOTORS_H
#define _MOTORS_H

#include <types.h>
#include <angles.h>
#include "single-motor.h"

#define DEBUG_MOTORS

#ifdef DEBUG_MOTORS
	#define MOTORS_DEBUG(...) Serial.print(__VA_ARGS__);
	#define MOTORS_DEBUGLN(...) Serial.println(__VA_ARGS__);
	#define MOTORS_DEBUGVAL(...) Serial.print(__VA_ARGS__); Serial.print("\t");
#else
	#define MOTORS_DEBUG(...)
	#define MOTORS_DEBUGLN(...)
	#define MOTORS_DEBUGVAL(...)
#endif


class Motors {
public:
Motors(structMotorConfig& c1, structMotorConfig& c2, structMotorConfig& c3, structPid* pid);
void init();
void reset();
void loop(unsigned long, double);

void updateEncoderMotor1(uint32_t value);
void updateEncoderMotor2(uint32_t value);
void updateEncoderMotor3(uint32_t value);

void setSpeed(float x, float y);

private:
SingleMotor m1;
SingleMotor m2;
SingleMotor m3;
SingleMotor* motors[3];
double holoAngle[3];

};

#endif
