
#include <types.h>
#include <motors.h>
#include <mpu-sensor.h>
#include <console.h>

structPid motorSpeedPid = {
  .p=0.1,
  .i=0.08,
  .d=0.0
};

structMotorConfig m1 = {2,3,32,30};
structMotorConfig m2 = {4,5,28,26};
structMotorConfig m3 = {6,7,29,27};

MPUSensor sensor(53);
Motors motors(m1, m2, m3, &motorSpeedPid);
Console console(&motors, &motorSpeedPid);

void setup(){
  Serial.begin(115200);

  sensor.init();
  motors.init();
  console.init();

  initEncoderInterupt();
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
  motors.loop(now, dtS);
  console.loop(now, dtS);
}




RoReg* registerEncoder = &g_APinDescription[32].pPort -> PIO_PDSR;

/**
 * Timer interupt handler
 */
void myHandler() {
  uint32_t value = *registerEncoder;
  motors.updateEncoders(value);
}

//TODO put this in motors.h and optimise (call only the good motor and not all of them)
void initEncoderInterupt() {
  attachInterrupt(32, myHandler, CHANGE);
  attachInterrupt(30, myHandler, CHANGE);
  attachInterrupt(28, myHandler, CHANGE);
  attachInterrupt(26, myHandler, CHANGE);
  attachInterrupt(29, myHandler, CHANGE);
  attachInterrupt(27, myHandler, CHANGE);
  // Timer.getAvailable().attachInterrupt(myHandler).setFrequency(100000).start();
}
