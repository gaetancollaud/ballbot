#ifndef _STATUS_H
#define _STATUS_H

#include <Arduino.h>

#define STATUS_FACTOR 100

class Status
{
public:
    Status(int onBoardLed = 13);

    void init();

    void loop(unsigned long, double);

    void increaseSensorCount();

private:
    int ledPin;
    bool previousState;
    int counter = 0;

    unsigned long nextFps = 0;
    int fpsCount = 0;
    int sensorCount = 0;
};

#endif
