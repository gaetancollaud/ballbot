
#include <types.h>
#include <angles.h>
#include <motors.h>
#include <mpu-sensor.h>
#include <console.h>
#include <position.h>

structPid motorSpeedPid = {.p=0.1, .i=0.05, .d=0.0};
structPid balancePid = {.p=300.0, .i=14.0, .d=600.0};
// structPid balancePid = {.p=100.0, .i=20.0, .d=0.0};
structPid positionPid = {.p=0.000, .i=0.0, .d=0.0};

structPid* pidToDebug = &balancePid;

RoReg* registerEncoder = &g_APinDescription[32].pPort->PIO_PDSR;
structMotorConfig m1 = {2,3,32,30, registerEncoder, -PI_60};
structMotorConfig m2 = {4,5,28,26, registerEncoder, PI_60};
structMotorConfig m3 = {6,7,29,27, registerEncoder, PI_180};

MPUSensor sensor(53);
Motors motors(m1, m2, m3, &motorSpeedPid);
Balance balance(&sensor, &motors, &balancePid);
Position position(&balance, &motors, &positionPid);
Console console(&motors, &balance, &position, pidToDebug, &positionPid);

void setup(){
	Serial.begin(115200);

	motors.init();
	sensor.init();
	balance.init();
	position.init();
	console.init();

	sensor.reset();

	initEncoderInterupt();
}

unsigned long lastLoop = 0;
unsigned nextFps = 0;
unsigned nextPid= 0;
int fpsCount = 0;
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
	position.loop(now, dtS);
	balance.loop(now, dtS);
	motors.loop(now, dtS);
	console.loop(now, dtS);

	//fps
	fpsCount++;
	if(now>nextFps) {
		nextFps = now + 1000;
		Serial.print("FPS: ");
		Serial.println(fpsCount);
		fpsCount = 0;
	}
	if(now>nextPid) {
		nextPid = now + 5000;
		Serial.print("PID: ");
		Serial.print(pidToDebug->p);
		Serial.print("\t");
		Serial.print(pidToDebug->i);
		Serial.print("\t");
		Serial.println(pidToDebug->d);
	}

}

void motor1Handler() {
	motors.updateEncoderMotor1(*registerEncoder);
}
void motor2Handler() {
	motors.updateEncoderMotor2(*registerEncoder);
}
void motor3Handler() {
	motors.updateEncoderMotor3(*registerEncoder);
}

//TODO put this in motors.h and optimise (call only the good motor and not all of them)
void initEncoderInterupt() {
	attachInterrupt(m1.rotaryA, motor1Handler, CHANGE);
	attachInterrupt(m1.rotaryB, motor1Handler, CHANGE);
	attachInterrupt(m2.rotaryA, motor2Handler, CHANGE);
	attachInterrupt(m2.rotaryB, motor2Handler, CHANGE);
	attachInterrupt(m3.rotaryA, motor3Handler, CHANGE);
	attachInterrupt(m3.rotaryB, motor3Handler, CHANGE);
	// Timer.getAvailable().attachInterrupt(myHandler).setFrequency(100000).start();
}
