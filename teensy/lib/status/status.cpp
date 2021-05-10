#include "status.h"

Status::Status(Console *console, int ledPin) : console(console), ledPin(ledPin)
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
        // Serial.println(fpsCount);
        this->console->sendItem("fps", fpsCount);
        fpsCount = 0;
    }

    this->counter++;
    if (this->counter >= STATUS_FACTOR)
    {
        this->previousState = !this->previousState;
        this->counter = 0;
        digitalWrite(this->ledPin, this->previousState ? HIGH : LOW);
    }
}
