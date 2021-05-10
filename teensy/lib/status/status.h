#ifndef _STATUS_H
#define _STATUS_H

#include <Arduino.h>
#include <console.h>

#define STATUS_FACTOR 10000

class Status
{
public:
    Status(Console *console, int onBoardLed = 13);

    void init();

    void loop(unsigned long, double);

private:
    Console *console;
    int ledPin;
    bool previousState;
    int counter = 0;

    unsigned long nextFps = 0;
    int fpsCount = 0;
    int sensorCount = 0;
};

#endif
