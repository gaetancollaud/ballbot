#ifndef _STATUS_H
#define _STATUS_H

#include <Arduino.h>
#include <console.h>
#include <mpu-sensor.h>

#define STATUS_LED_FACTOR 100000
#define UPDATE_INTERVAL_MS 50

class Status
{
public:
    Status(Console *console, MPUSensor* sensor, int onBoardLed = 13);

    void init();

    void loop(unsigned long, double);

private:
    Console *console;
    MPUSensor* sensor;
    int ledPin;
    bool ledState;
    int ledCounter = 0;

    unsigned long nextFps = 0;
    unsigned long nextUpdate = 0;
    int fpsCount = 0;
};

#endif
