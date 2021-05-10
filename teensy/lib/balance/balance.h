#ifndef _BALANCE_H
#define _BALANCE_H

#include <motors.h>
#include <mpu-sensor.h>
#include <pid.h>

//166Hz
#define BALANCE_DELAY_MS 10

class Balance
{
public:
    Balance(MPUSensor *sensor, Motors *motors, structPid *pidBalance);
    void init();
    void reset();
    void loop(unsigned long, double);
    void toggleEnable();
    void setMaxIntegral(double v);
    void setMaxOutput(double v);
    void setTargetAngle(double x, double y);

private:
    MPUSensor *sensor;
    Motors *motors;
    structPid *pidBalance;
    double accDts;

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
