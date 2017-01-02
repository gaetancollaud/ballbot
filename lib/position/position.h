#ifndef _POSITION_H
#define _POSITION_H

#include <balance.h>
#include <motors.h>
#include <pid.h>

#define POSITION_DELAY_MS 10
#define POSITION_DELAY_S 0.010

class Position {
public:
Position(Balance* balance, Motors* motors,  structPid* pidPosition);
void init();
void reset();
void loop(unsigned long, double);
void toggleEnable();

private:
Balance* balance;
Motors* motors;
structPid* pidPosition;

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
