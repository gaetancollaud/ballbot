#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <motors.h>
#include <balance.h>
#include <position.h>

class Console {
public:
Console(Motors* motors, Balance* balance, Position* position, structPid* balancePid, structPid* positionPid);
void init();
void loop(unsigned long, double);

private:
Motors* motors;
Balance* balance;
Position* position;
structPid* balancePid;
structPid* positionPid;

String inputString;

void analyse();
float getFloatValue();

};

#endif
