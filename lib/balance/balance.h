#ifndef _BALANCE_H
#define _BALANCE_H

#include <motors.h>
#include <mpu-sensor.h>
#include <pid.h>

#define BALANCE_DELAY_MS 10
#define BALANCE_DELAY_S 0.010

class Balance {
public:
Balance(MPUSensor* sensor, Motors* motors, structPid* pidBalance);
void init();
void reset();
void loop(unsigned long, double);
void toggleEnable();

private:
MPUSensor* sensor;
Motors* motors;
structPid* pidBalance;

bool enable;
unsigned long nextTime;

PID pidBalanceX;
PID pidBalanceY;

double outputBalanceX;
double outputBalanceY;
double targetBalanceX;
double targetBalanceY;

};

#endif
