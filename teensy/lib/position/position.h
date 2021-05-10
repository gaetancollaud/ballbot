#ifndef _POSITION_H
#define _POSITION_H

#include <balance.h>
#include <motors.h>
#include <pid.h>

// #define DEBUG_POSITION

#ifdef DEBUG_POSITION
#define POSITION_DEBUG(...) Serial.print(__VA_ARGS__);
#define POSITION_DEBUGLN(...) Serial.println(__VA_ARGS__);
#define POSITION_DEBUGVAL(...) \
	Serial.print(__VA_ARGS__); \
	Serial.print("\t");
#else
#define POSITION_DEBUG(...)
#define POSITION_DEBUGLN(...)
#define POSITION_DEBUGVAL(...)
#endif

#define POSITION_DELAY_MS 100
#define POSITION_DELAY_S 0.100

class Position
{
public:
	Position(Balance *balance, Motors *motors, structPid *pidPosition);
	void init();
	void reset();
	void loop(unsigned long, double);
	void toggleEnable();

private:
	Balance *balance;
	Motors *motors;
	structPid *pidPosition;

	unsigned long nextTime;
	bool enable;

	PID pidPositionX;
	PID pidPositionY;

	double outputPositionX;
	double outputPositionY;
	double targetPositionX;
	double targetPositionY;
};

#endif
