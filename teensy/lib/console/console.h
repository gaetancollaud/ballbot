#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <motors.h>
#include <balance.h>
#include <position.h>

#define CONSOLE_SERIAL Serial2

class Console
{
public:
    Console(Motors *motors, Balance *balance, Position *position, structPid *balancePid, structPid *positionPid);
    void init();
    void loop(unsigned long, double);
    void sendItem(String item, String value);
    void sendItem(String item, int value);
    void sendItem(String item, double value);

private:
    Motors *motors;
    Balance *balance;
    Position *position;
    structPid *balancePid;
    structPid *positionPid;

    String inputString;

    void analyse();
    float getFloatValue();
};

#endif
