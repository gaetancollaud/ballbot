#include "mpu-sensor.h"

MPUSensor::MPUSensor(int pinCS) : pinCS(pinCS) {
	// this->zax = 0;
	// this->zay = 0;
	// this->zaz = 0;
	// this->zgx = 0;
	// this->zgy = 0;
	// this->zgz = 0;
}

void MPUSensor::init() {
	pinMode(pinCS, OUTPUT);
	digitalWrite(pinCS, LOW);
	delay(100);
	digitalWrite(pinCS, HIGH);

	//TODO try to speed up i2c to 400khz

	Wire.begin();
	accelgyro.initialize();
	if (!accelgyro.testConnection()) {
		Serial.println("Cannot connect to MPU6050");
	}

	accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_500);
	MPU_DEBUGLN("Sensor initilized");
}

void MPUSensor::reset() {
}

void MPUSensor::loop(unsigned long now, double dtS) {
	// this->refreshSensors();
	// this->computeAngles(dtS);
}

double* MPUSensor::getAngleXptr() {
	return &this->angleX;
}

double* MPUSensor::getAngleYptr() {
	return &this->angleY;
}
