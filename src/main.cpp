#include <Arduino.h>

#include <mpu-sensor.h>
#include <console.h>
#include <status.h>
#include <motors.h>

void initEncoderInterupt();

structPid motorSpeedPid = {.p = 200, .i = 5, .d = 0};

structMotorConfig m1 = {10, 11, 2, 3, -PI_60};
structMotorConfig m2 = {10, 11, 4, 6, PI_60};
structMotorConfig m3 = {10, 11, 7, 8, PI_180};

Status status(13);
MPUSensor sensor(&status, 23);
// Motors motors(m1, m2, m3, &motorSpeedPid);
SingleMotor motor1(m1, &motorSpeedPid);

void dmpDataReady()
{
  sensor.dmpDataReady();
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);// wait for terminal to connect

  // sensor.init(&dmpDataReady);
  motor1.init();
  status.init();

  sensor.reset();

  initEncoderInterupt();
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

  // sensor.loop(now, dtS);
  motor1.loop(now, dtS);
  status.loop(now, dtS);

  // position.loop(now, dtS);
  // balance.loop(now, dtS);
  // motors.loop(now, dtS);
  // console.loop(now, dtS);


  motor1.motorTest(now, dtS); //TODO remove
}

void motor1HandlerA() {
  motor1.updateEncoderA();
	// motors.updateEncoderMotor1();
}
void motor1HandlerB() {
  motor1.updateEncoderB();
	// motors.updateEncoderMotor1();
}
// void motor2Handler() {
// 	motors.updateEncoderMotor2();
// }
// void motor3Handler() {
// 	motors.updateEncoderMotor3();
// }

void initEncoderInterupt() {
	attachInterrupt(m1.rotaryA, motor1HandlerA, CHANGE);
	// attachInterrupt(m1.rotaryB, motor1HandlerB, CHANGE);
	// attachInterrupt(m2.rotaryA, motor2Handler, CHANGE);
	// attachInterrupt(m2.rotaryB, motor2Handler, CHANGE);
	// attachInterrupt(m3.rotaryA, motor3Handler, CHANGE);
	// attachInterrupt(m3.rotaryB, motor3Handler, CHANGE);
}