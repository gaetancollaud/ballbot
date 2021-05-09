#ifndef _SINGLE_MOTOR_H
#define _SINGLE_MOTOR_H

#include <types.h>
#include <Arduino.h>
#include <pid.h>

#define MAX_DC_MOTOR_VALUE 255
#define REGULATION_INTERVAL_MS 5
#define REGULATION_INTERVAL_S 0.005
#define MOTOR_MIN_SPEED 0

#define DEBUG_MOTOR

#ifdef DEBUG_MOTOR
#define MOTOR_DEBUG(...) Serial.print(__VA_ARGS__);
#define MOTOR_DEBUGLN(...) Serial.println(__VA_ARGS__);
#define MOTOR_DEBUGVAL(...)    \
	Serial.print(__VA_ARGS__); \
	Serial.print("\t");
#else
#define MOTOR_DEBUG(...)
#define MOTOR_DEBUGLN(...)
#define MOTOR_DEBUGVAL(...)
#endif


typedef void (*encoderInterrupFunction)(void);

class SingleMotor
{
public:
	SingleMotor(structMotorConfig &motorConfig, structPid *pidValue);

	void init();
	void reset();
	void setSpeed(double speed);
	void loop(unsigned long nowMs, double dtS);
	void updateEncoderA();
	void updateEncoderB();
	void motorTest(unsigned long nowMs, double dtS);

private:
	structMotorConfig config;

	PID pidSpeed;

	double targetSpeed;
	double currentSpeed;
	double outputSpeed;
	structPid pidValue;

	unsigned long nextTime;
	volatile long encoderValue;
	long lastEncoderValue;
	int lastEncoded;
	long encoderCount;
	long lastEncoderCount;

	int maskRotaryA;
	int maskRotaryB;

	void writeOutputSpeed();
};

#endif
