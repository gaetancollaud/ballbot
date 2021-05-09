#include "status.h"

Status::Status(int ledPin) : ledPin(ledPin)
{
}

void Status::init()
{
    pinMode(this->ledPin, OUTPUT);
}

void Status::loop(unsigned long nowMs, double dtS)
{

    //fps
    fpsCount++;
    if (nowMs > nextFps)
    {
        nextFps = nowMs + 1000;
        // Serial.print("FPS: ");
        // Serial.print(fpsCount);
        // Serial.print("\tSensor: ");
        // Serial.println(sensorCount);
        fpsCount = 0;
        sensorCount = 0;
    }

    this->counter++;
    if (this->counter >= STATUS_FACTOR)
    {
        this->previousState = !this->previousState;
        this->counter = 0;
        digitalWrite(this->ledPin, this->previousState ? HIGH : LOW);
    }
}

void Status::increaseSensorCount(){
    this->sensorCount++;
}
