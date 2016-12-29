
#include <motors.h>
#include <mpu-sensor.h>
#include <types.h>

structPid motorSpeedPid = {
  .p=0.5,
  .i=15.0,
  .d=0.0
};

structMotorConfig motorConfig[] = {
  {2,3,32,30},
  {4,5,28,26},
  {6,7,29,27}
};

// DCMotor motor1(2, 3, 32 ,30, &pidValue);
// DCMotor motor2(4, 5, 28, 26, &pidValue);
// DCMotor motor3(6, 7, 29 ,27, &pidValue);


MPUSensor sensor(53);
Motors motors(motorConfig, &motorSpeedPid);

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
