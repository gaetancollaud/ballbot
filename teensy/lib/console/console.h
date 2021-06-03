#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <motors.h>
#include <balance.h>
#include <position.h>

#define CONSOLE_SERIAL Serial2


#define DEBUG_CONSOLE

#ifdef DEBUG_CONSOLE
#define CONSOLE_DEBUG(...) Serial.print(__VA_ARGS__);
#define CONSOLE_DEBUGLN(...) Serial.println(__VA_ARGS__);
#define CONSOLE_DEBUGVAL(...)      \
	Serial.print(__VA_ARGS__); \
	Serial.print("\t");
#define CONSOLE_DEBUGF(...)           \
	char buffer[100];             \
	sprintf(buffer, __VA_ARGS__); \
	Serial.print(buffer);
#else
#define CONSOLE_DEBUG(...)
#define CONSOLE_DEBUGLN(...)
#define CONSOLE_DEBUGVAL(...)
#define CONSOLE_DEBUGF(...)
#endif

class Console
{
public:
    Console(Motors *motors, Balance *balance, Position *position, structPid *balancePid, structPid *positionPid, structPid *motorPid);
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
    structPid *motorPid;

    String inputString;

    void analyse();
    void handleValue(String item, String value);
};

#endif
