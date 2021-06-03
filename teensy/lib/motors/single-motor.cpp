#include "single-motor.h"

SingleMotor::SingleMotor(structMotorConfig &config, structPid *pidValue)
	: config(config), pidSpeed(&currentSpeed, &outputSpeed, &targetSpeed, pidValue)
{
	this->encoderValue = 0;
	this->lastEncoderValue = 0;
	this->targetSpeed = 0;

	pidSpeed.setLimitOutput(MAX_DC_MOTOR_VALUE);
}

void SingleMotor::init()
{

	pinMode(this->config.pinLeft, OUTPUT);
	pinMode(this->config.pinRight, OUTPUT);

	//!\ Hardware pullup of 10k must be present, Arduino DUE pullup are too big
	pinMode(this->config.rotaryA, INPUT_PULLUP);
	pinMode(this->config.rotaryB, INPUT_PULLUP);

	analogWrite(this->config.pinLeft, LOW);
	analogWrite(this->config.pinRight, LOW);

	MOTOR_DEBUGLN("Motor initilized");
}

void SingleMotor::reset()
{
	this->pidSpeed.reset();
}

void SingleMotor::setSpeed(double speed)
{
	this->targetSpeed = map(speed, -100, 100, -1000, 1000);
	Serial.print("Set speed to ");
	Serial.println(this->targetSpeed);
	// this->targetSpeed = constrain(speed, -100, 100);
}

void SingleMotor::loop(unsigned long nowMs, double dtS)
{
	if (nowMs >= this->nextTime)
	{
		this->nextTime = nowMs + REGULATION_INTERVAL_MS;

		long ev = this->encoderValue;
		// this->currentSpeed = (ev - this->lastEncoderValue) / dtS;
		this->currentSpeed = (ev - this->lastEncoderValue) / REGULATION_INTERVAL_S;
		this->lastEncoderValue = ev;

		long ec = this->encoderCount;
		long ecDiff = ec - this->lastEncoderCount;
		this->lastEncoderCount = ec;

		// this->pidSpeed.loop(nowMs, dtS);
		this->pidSpeed.loop(nowMs, REGULATION_INTERVAL_S);

		this->writeOutputSpeed();

		MOTOR_DEBUGF("encoder: %d\tcount: %d\tcurrent: %5.0f\ttarget: %5.0f\toutput: %3.2f", ev, ecDiff, this->currentSpeed, this->targetSpeed, this->outputSpeed)
		MOTOR_DEBUGLN()
		

		// MOTOR_DEBUG("encoder: ");
		// MOTOR_DEBUG(ev);
		// MOTOR_DEBUG("\tcount: ");
		// MOTOR_DEBUG(ecDiff);
		// MOTOR_DEBUG("\tcurrent: ");
		// MOTOR_DEBUG(this->currentSpeed);
		// MOTOR_DEBUG("\ttarget: ");
		// MOTOR_DEBUG(this->targetSpeed);
		// MOTOR_DEBUG("\toutput: ");
		// MOTOR_DEBUG(this->outputSpeed);
		// MOTOR_DEBUGLN();
	}
}

void SingleMotor::updateEncoderA()
{
	encoderCount++;

	if (digitalRead(config.rotaryA) == digitalRead(config.rotaryB))
	{
		encoderValue++;
	}
	else
	{
		encoderValue--;
	}
}

void SingleMotor::updateEncoderB()
{
	encoderCount++;

	if (digitalRead(config.rotaryA) == digitalRead(config.rotaryB))
	{
		encoderValue--;
	}
	else
	{
		encoderValue++;
	}
}

void SingleMotor::writeOutputSpeed()
{
	int speed = this->outputSpeed;
	int16_t l = 0, r = 0;

	if (speed == 0)
	{
		r = LOW;
		l = LOW;
	}
	else if (speed > 0)
	{
		speed += MOTOR_MIN_SPEED;
		r = speed;
		l = LOW;
	}
	else
	{
		speed -= MOTOR_MIN_SPEED;
		r = LOW;
		l = -speed;
	}
	analogWrite(this->config.pinLeft, l);
	analogWrite(this->config.pinRight, r);
}

void SingleMotor::motorTest(unsigned long nowMs, double dtS)
{
	int state = (nowMs / 1000) % 4;

	switch (state)
	{
	case 0:
		this->setSpeed(0);
		break;
	case 1:
		this->setSpeed(100);
		break;
	case 2:
		this->setSpeed(0);
		break;
	case 3:
		this->setSpeed(-100);
		break;
	}
}