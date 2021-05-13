#include "status.h"

Status::Status(Console *console, MPUSensor *sensor, int ledPin) : console(console), sensor(sensor), ledPin(ledPin)
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

    // update
    if (nowMs > nextUpdate)
    {
        nextUpdate = nowMs + UPDATE_INTERVAL_MS;
        this->console->sendItem("angleX", *this->sensor->getAngleXptr());
        this->console->sendItem("angleY", *this->sensor->getAngleYptr());
        this->console->sendItem("angleZ", *this->sensor->getAngleZptr());
    }

    this->ledCounter++;
    if (this->ledCounter >= STATUS_LED_FACTOR)
    {
        this->ledState = !this->ledState;
        this->ledCounter = 0;
        digitalWrite(this->ledPin, this->ledState ? HIGH : LOW);
    }
}
