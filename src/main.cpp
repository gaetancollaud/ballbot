#include <Arduino.h>
#include <mpu-sensor.h>
#include <console.h>
#include <status.h>

MPUSensor sensor(22, 23);
Status status(13);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000); // let time to connect

  sensor.init();
  status.init();

  sensor.reset();
}

unsigned long lastLoop = 0;
unsigned nextFps = 0;
unsigned nextPid = 0;
int fpsCount = 0;
void loop()
{
  // delay(1);

  unsigned long now = millis();
  unsigned long dtMs = 1;
  if (lastLoop < now)
  {
    //or else overflow, keep 1ms as arbitratry dtMs
    dtMs = now - lastLoop;
  }
  lastLoop = now;
  double dtS = ((double)dtMs) * 0.001;

  // sensor.detectDevices();

  sensor.loop(now, dtS);
  status.loop(now, dtS);
  // position.loop(now, dtS);
  // balance.loop(now, dtS);
  // motors.loop(now, dtS);
  // console.loop(now, dtS);
}