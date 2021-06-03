#include <Arduino.h>

#include <mpu-sensor.h>
#include <console.h>
#include <status.h>
#include <motors.h>

void initEncoderInterupt();

structPid motorSpeedPid = {.p = 200, .i = 5, .d = 0};
structPid balancePid = {.p=300.0, .i=14.0, .d=600.0};
structPid positionPid = {.p=0.000, .i=0.0, .d=0.0};

structMotorConfig m1 = {17, 16, 12, 11, -PI_60};
structMotorConfig m2 = {38, 37, 25, 24, PI_60};
structMotorConfig m3 = {36, 35, 30, 29, PI_180};

MPUSensor sensor(23);
Motors motors(m1, m2, m3, &motorSpeedPid);
Balance balance(&sensor, &motors, &balancePid);
Position position(&balance, &motors, &positionPid);
Console console(&motors, &balance, &position, &balancePid, &positionPid, &motorSpeedPid);
Status status(&console, &sensor, 13);

void dmpDataReady()
{
  sensor.dmpDataReady();
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000); // wait for terminal to connect

  
	motors.init();
  sensor.init(&dmpDataReady);
	balance.init();
	position.init();
	console.init();
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

	sensor.loop(now, dtS);
	position.loop(now, dtS);
	balance.loop(now, dtS);
	motors.loop(now, dtS);
	console.loop(now, dtS);
  status.loop(now, dtS);

}

void motor1HandlerA()
{
  motors.updateEncoderMotor1A();
}
void motor1HandlerB()
{
  motors.updateEncoderMotor1B();
}
void motor2HandlerA() {
	motors.updateEncoderMotor2A();
}
void motor2HandlerB() {
	motors.updateEncoderMotor2B();
}
void motor3HandlerA() {
	motors.updateEncoderMotor3A();
}
void motor3HandlerB() {
	motors.updateEncoderMotor3B();
}

void initEncoderInterupt()
{
  attachInterrupt(m1.rotaryA, motor1HandlerA, CHANGE);
  attachInterrupt(m1.rotaryB, motor1HandlerB, CHANGE);
  attachInterrupt(m2.rotaryA, motor2HandlerA, CHANGE);
  attachInterrupt(m2.rotaryB, motor2HandlerB, CHANGE);
  attachInterrupt(m3.rotaryA, motor3HandlerA, CHANGE);
  attachInterrupt(m3.rotaryB, motor3HandlerB, CHANGE);
}