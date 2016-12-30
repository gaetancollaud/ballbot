#ifndef _TYPES_H
#define _TYPES_H

#include <Arduino.h>

typedef struct {
	double p;
	double i;
	double d;
} structPid;

typedef struct {
	double x;
	double y;
} structPos;

typedef struct {
	int pinLeft;
	int pinRight;
	int rotaryA;
	int rotaryB;
	RoReg* registerEncoder;
} structMotorConfig;

#endif
