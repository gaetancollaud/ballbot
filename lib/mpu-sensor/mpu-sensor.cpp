#include "mpu-sensor.h"

MPUSensor::MPUSensor(int pinCS) : pinCS(pinCS) {
	this->zax = 0;
	this->zay = 0;
	this->zaz = 0;
	this->zgx = 0;
	this->zgy = 0;
	this->zgz = 0;
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
	double axSum = 0, aySum = 0, azSum = 0, gxSum = 0, gySum = 0, gzSum = 0;
	for (int i = 0; i < RESET_LOOP; i++) {
		accelgyro.getMotion6(&this->ax, &this->ay, &this->az, &this->gx, &this->gy, &this->gz);
		axSum += this->ax;
		aySum += this->ay;
		azSum += this->az;
		gxSum += this->gx;
		gySum += this->gy;
		gzSum += this->gz;
		delay(10);
	}

	double factor = 1.0 / RESET_LOOP;
	this->zax = axSum*factor;
	this->zay = aySum*factor;
	this->zaz = azSum*factor;
	this->zgx = gxSum*factor;
	this->zgy = gySum*factor;
	this->zgz = gzSum*factor;

	this->zaz += 16384;

	//	MPU_DEBUG(this->zax);
	//	MPU_DEBUG(this->zay);
	//	MPU_DEBUG(this->zaz);
	//	MPU_DEBUG(this->zgx);
	//	MPU_DEBUG(this->zgy);
	//	MPU_DEBUG(this->zgz);
	//	MPU_DEBUGLN("");
	//	while (true);
}

void MPUSensor::loop(unsigned long now, double dtS) {
	this->refreshSensors();
	this->computeAngles(dtS);
}

double* MPUSensor::getAngleXptr() {
	return &this->angleX;
}

double* MPUSensor::getAngleYptr() {
	return &this->angleY;
}

void MPUSensor::refreshSensors() {
	int loop = 5;
	double axSum = 0, aySum = 0, azSum = 0, gxSum = 0, gySum = 0, gzSum = 0;
	for (int i = 0; i < loop; i++) {
		accelgyro.getMotion6(&this->ax, &this->ay, &this->az, &this->gx, &this->gy, &this->gz);
		axSum += this->ax;
		aySum += this->ay;
		azSum += this->az;
		gxSum += this->gx;
		gySum += this->gy;
		gzSum += this->gz;
	}

	double factor = 1.0 / loop;
	this->ax = axSum*factor;
	this->ay = aySum*factor;
	this->az = azSum*factor;
	this->gx = gxSum;
	this->gy = gySum;
	this->gz = gzSum;
	// accelgyro.getMotion6(&this->ax, &this->ay, &this->az, &this->gx, &this->gy, &this->gz);

	if (
		abs(this->ax) >= ALLERT_TRESHOLD ||
		abs(this->ay) >= ALLERT_TRESHOLD ||
		abs(this->az) >= ALLERT_TRESHOLD) {
		Serial.println("/!\\ MPU6050 Accel limits !!!");
	}

	if (
		abs(this->gx) >= ALLERT_TRESHOLD ||
		abs(this->gy) >= ALLERT_TRESHOLD ||
		abs(this->gz) >= ALLERT_TRESHOLD) {
		Serial.println("/!\\ MPU6050 Gyro limits !!!");
	}


	this->ax -= this->zax;
	this->ay -= this->zay;
	this->az -= this->zaz;
	this->gx -= this->zgx;
	this->gy -= this->zgy;
	this->gz -= this->zgz;

	this->ax = -this->ax;
	this->ay = -this->ay;
	this->az = -this->az;

	//		MPU_DEBUG(this->ax);
	//		MPU_DEBUG(this->ay);
	//		MPU_DEBUG(this->az);
	//		MPU_DEBUG(this->gx);
	//		MPU_DEBUG(this->gy);
	//		MPU_DEBUG(this->gz);
	//	MPU_DEBUGLN();
}

//double sumGx = 0.0;
//double sumGy = 0.0;

void MPUSensor::computeAngles(double dtS) {
	double accZval = this->az;
	double accYangle = (atan2((double) this->ax, accZval));
	double accXangle = -(atan2((double) this->ay, accZval));

	double gyroXRate = -(this->gx * GYRO_RATIO * dtS);
	double gyroYRate = -(this->gy * GYRO_RATIO * dtS);

	//	sumGx += gyroXRate;
	//	sumGy += gyroYRate;

	angleX = GYRO_TRUST * (angleX + gyroXRate) + GYRO_REVERSE_TRUST * accXangle;
	angleY = GYRO_TRUST * (angleY + gyroYRate) + GYRO_REVERSE_TRUST * accYangle;

	MPU_DEBUG(String(dtS, 4));
	MPU_DEBUG("\t\t");

	MPU_DEBUG(String(toReadableDegr(accXangle), 1));
	MPU_DEBUG("\t");
	//	debug(String(toReadableDegr(sumGx), 1));
	//	debug("\t");
	MPU_DEBUG(String(toReadableDegr(angleX), 1));
	MPU_DEBUG("\t\t");
	MPU_DEBUG(String(toReadableDegr(accYangle), 1));
	MPU_DEBUG("\t");
	//	debug(String(toReadableDegr(sumGy), 1));
	//	debug("\t");
	MPU_DEBUGLN(String(toReadableDegr(angleY), 1));
}

double MPUSensor::constraintAngle(double *angle) {
	if (*angle > PI) {
		*angle -= TWO_PI;
	} else if (*angle<-PI) {
		*angle += TWO_PI;
	}
}

double MPUSensor::toReadableDegr(double v) {
	v *= 57.2957795131;
	while (v<-180.0) {
		v += 360.0;
	}
	while (v > 180) {
		v -= 360;
	}
	return v;
}
