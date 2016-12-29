#include <mpu-sensor.h>

MPUSensor sensor(53);

void setup(){
  Serial.begin(115200);

  sensor.init();

}

unsigned long lastLoop = 0;
void loop(){
  unsigned long now = millis();
  unsigned long dtMs = 1;
  if (lastLoop < now) {
      //or else overflow, keep 1ms as arbitratry dtMs
      dtMs = now - lastLoop;
  }
  lastLoop = now;

  double dtS = ((double)dtMs)/1000.0;

  sensor.loop(now, dtS);
}
