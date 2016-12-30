#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <motors.h>

class Console{
public:
	Console(Motors* motors, structPid* motorPid);
	void init();
	void loop(unsigned long, double);

private:
  Motors* motors;
  structPid* motorPid;

  String inputString;

  void analyse();
  void readSpeed();
  float getFloatValue();

};

#endif
