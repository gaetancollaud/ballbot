#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <motors.h>
#include <balance.h>

class Console {
public:
Console(Motors* motors, Balance* balance, structPid* motorPid);
void init();
void loop(unsigned long, double);

private:
Motors* motors;
Balance* balance;
structPid* motorPid;

String inputString;

void analyse();
void readSpeed();
float getFloatValue();

};

#endif
