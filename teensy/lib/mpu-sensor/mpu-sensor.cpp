#include "mpu-sensor.h"

#ifdef MODE_DMP
#include <MPU6050_6Axis_MotionApps20.h>
#endif

MPUSensor::MPUSensor(int pinInterupt) : pinInterupt(pinInterupt)
{
	this->accelgyro = new MPU6050();

#ifndef MODE_DMP
	this->zax = 0;
	this->zay = 0;
	this->zaz = 0;
	this->zgx = 0;
	this->zgy = 0;
	this->zgz = 0;
#endif
}

void MPUSensor::init(mpuInterrupFunction func)
{
	pinMode(this->pinInterupt, INPUT);

	//TODO try to speed up i2c to 400khz

	Wire.begin();
	Wire.setClock(400000);
	accelgyro->initialize();
	int retryLeft = 5;
	while (!accelgyro->testConnection() && retryLeft-- > 0)
	{
		Serial.println("Cannot connect to MPU6050");
		delay(1000);
	}

#ifdef MODE_DMP

	devStatus = accelgyro->dmpInitialize();

	// supply your own gyro offsets here, scaled for min sensitivity
	accelgyro->setXGyroOffset(220);
	accelgyro->setYGyroOffset(76);
	accelgyro->setZGyroOffset(-85);
	accelgyro->setZAccelOffset(1788);

	if (devStatus == 0)
	{
		// Calibration Time: generate offsets and calibrate our MPU6050
		// accelgyro->CalibrateAccel(6);
		// accelgyro->CalibrateGyro(6);
		// accelgyro->PrintActiveOffsets();
		// turn on the DMP, now that it's ready
		Serial.println(F("Enabling DMP..."));
		accelgyro->setDMPEnabled(true);

		// enable Arduino interrupt detection
		Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
		Serial.print(digitalPinToInterrupt(this->pinInterupt));
		Serial.println(F(")..."));
		attachInterrupt(digitalPinToInterrupt(this->pinInterupt), func, RISING);
		mpuIntStatus = accelgyro->getIntStatus();

		// set our DMP Ready flag so the main loop() function knows it's okay to use it
		Serial.println(F("DMP ready! Waiting for first interrupt..."));
		dmpReady = true;

		// get expected DMP packet size for later comparison
		packetSize = accelgyro->dmpGetFIFOPacketSize();
	}
	else
	{
		// ERROR!
		// 1 = initial memory load failed
		// 2 = DMP configuration updates failed
		// (if it's going to break, usually the code will be 1)
		Serial.print(F("DMP Initialization failed (code "));
		Serial.print(devStatus);
		Serial.println(F(")"));
	}
#endif
#ifndef MODE_DMP
	accelgyro->setFullScaleGyroRange(MPU6050_GYRO_FS_500);
#endif

	// accelgyro->setFullScaleGyroRange(MPU6050_GYRO_FS_500);
	MPU_DEBUGLN("Sensor initilized");
}

void MPUSensor::reset()
{
#ifndef MODE_DMP

	double axSum = 0, aySum = 0, azSum = 0, gxSum = 0, gySum = 0, gzSum = 0;
	for (int i = 0; i < RESET_LOOP; i++)
	{
		accelgyro->getMotion6(&this->ax, &this->ay, &this->az, &this->gx, &this->gy, &this->gz);
		axSum += this->ax;
		aySum += this->ay;
		azSum += this->az;
		gxSum += this->gx;
		gySum += this->gy;
		gzSum += this->gz;
		delay(10);
	}

	double factor = 1.0 / RESET_LOOP;
	this->zax = axSum * factor;
	this->zay = aySum * factor;
	this->zaz = azSum * factor;
	this->zgx = gxSum * factor;
	this->zgy = gySum * factor;
	this->zgz = gzSum * factor;

	this->zaz += 16384;
#endif
}

void MPUSensor::loop(unsigned long now, double dtS)
{
	this->refreshSensors();
	this->computeAngles(dtS);
}

void MPUSensor::dmpDataReady()
{
#ifdef MODE_DMP
	this->mpuInterrupt = true;
#endif
}

double *MPUSensor::getAngleXptr()
{
	return &this->angleX;
}

double *MPUSensor::getAngleYptr()
{
	return &this->angleY;
}

double *MPUSensor::getAngleZptr()
{
	return &this->angleZ;
}

void MPUSensor::detectDevices()
{

	byte error, address; //variable for error and I2C address
	int nDevices;

	Serial.println("Scanning...");

	nDevices = 0;
	for (address = 1; address < 127; address++)
	{
		// The i2c_scanner uses the return value of
		// the Write.endTransmisstion to see if
		// a device did acknowledge to the address.
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (error == 0)
		{
			Serial.print("I2C device found at address 0x");
			if (address < 16)
				Serial.print("0");
			Serial.print(address, HEX);
			Serial.println("  !");
			nDevices++;
		}
		else if (error == 4)
		{
			Serial.print("Unknown error at address 0x");
			if (address < 16)
				Serial.print("0");
			Serial.println(address, HEX);
		}
	}
	if (nDevices == 0)
		Serial.println("No I2C devices found\n");
	else
		Serial.println("done\n");

	delay(5000); // wait 5 seconds for the next I2C scan
}

#ifdef MODE_DMP
void MPUSensor::refreshSensors()
{

	if (!dmpReady)
	{
		return;
	}

	if (this->mpuInterrupt || fifoCount >= packetSize)
	{
		mpuInterrupt = false;
		mpuIntStatus = accelgyro->getIntStatus();
		fifoCount = accelgyro->getFIFOCount();

		if ((mpuIntStatus & 0x10) || fifoCount == 1024)
		{
			// reset so we can continue cleanly
			accelgyro->resetFIFO();
			Serial.println(F("FIFO overflow!"));

			// otherwise, check for DMP data ready interrupt (this should happen frequently)
		}
		else if ((mpuIntStatus & 0x02) > 0)
		{
			// wait for correct available data length, should be a VERY short wait
			while (fifoCount < packetSize)
				fifoCount = accelgyro->getFIFOCount();

			// read a packet from FIFO
			accelgyro->getFIFOBytes(fifoBuffer, packetSize);

			// track FIFO count here in case there is > 1 packet available
			// (this lets us immediately read more without waiting for an interrupt)
			fifoCount -= packetSize;

			float eulerValues[3];
			accelgyro->dmpGetQuaternion(&q, fifoBuffer);
			// accelgyro->dmpGetEuler(eulerValues, &q);

			accelgyro->dmpGetGravity(&gravity, &q);
			accelgyro->dmpGetYawPitchRoll(eulerValues, &q, &gravity);

			this->angleX = eulerValues[2];
			this->angleY = eulerValues[1];
			this->angleZ = eulerValues[0];

			MPU_DEBUGF("x=%.3f, y=%.3f, z=%.3f\n", this->angleX, this->angleY, this->angleZ)
		}
	}
}

void MPUSensor::computeAngles(double) {}
#endif

#ifndef MODE_DMP

void MPUSensor::refreshSensors()
{
	accelgyro->getMotion6(&this->ax, &this->ay, &this->az, &this->gx, &this->gy, &this->gz);

	// if (
	// 	abs(this->ax) >= ALLERT_TRESHOLD ||
	// 	abs(this->ay) >= ALLERT_TRESHOLD ||
	// 	abs(this->az) >= ALLERT_TRESHOLD)
	// {
	// 	Serial.println("/!\\ MPU6050 Accel limits !!!");
	// }

	// if (
	// 	abs(this->gx) >= ALLERT_TRESHOLD ||
	// 	abs(this->gy) >= ALLERT_TRESHOLD ||
	// 	abs(this->gz) >= ALLERT_TRESHOLD)
	// {
	// 	Serial.println("/!\\ MPU6050 Gyro limits !!!");
	// }

	this->ax -= this->zax;
	this->ay -= this->zay;
	this->az -= this->zaz;
	this->gx -= this->zgx;
	this->gy -= this->zgy;
	this->gz -= this->zgz;

	this->ax = -this->ax;
	this->ay = -this->ay;
	this->az = -this->az;

	// MPU_DEBUG(this->ax);
	// MPU_DEBUG(this->ay);
	// MPU_DEBUG(this->az);
	// MPU_DEBUG(this->gx);
	// MPU_DEBUG(this->gy);
	// MPU_DEBUG(this->gz);
	// MPU_DEBUGLN();
}

void MPUSensor::computeAngles(double dtS)
{
	double accZval = this->az;
	double accYangle = -(atan2((double)this->ax, accZval));
	double accXangle = (atan2((double)this->ay, accZval));

	double gyroXRate = -(this->gx * GYRO_RATIO * dtS);
	double gyroYRate = -(this->gy * GYRO_RATIO * dtS);

	//	sumGx += gyroXRate;
	//	sumGy += gyroYRate;

	double finalX = GYRO_TRUST * (angleX + gyroXRate) + GYRO_REVERSE_TRUST * accXangle;
	double finalY = GYRO_TRUST * (angleY + gyroYRate) + GYRO_REVERSE_TRUST * accYangle;

	//BEGIN sliding average
	slidingAverageX[slidingAverageIndex] = finalX;
	slidingAverageY[slidingAverageIndex] = finalY;

	double sumX = 0;
	double sumY = 0;
	for (int i = 0; i < AVERAGE_SLIDE; i++)
	{
		sumX += slidingAverageX[i];
		sumY += slidingAverageY[i];
	}

	angleX = sumX * AVERAGE_SLIDE_REVERSE;
	angleY = sumY * AVERAGE_SLIDE_REVERSE;

	slidingAverageIndex++;
	if (slidingAverageIndex >= AVERAGE_SLIDE)
	{
		slidingAverageIndex = 0;
	}
	//END sliding average

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

double MPUSensor::constraintAngle(double *angleRad)
{
	if (*angleRad > PI)
	{
		*angleRad -= TWO_PI;
	}
	else if (*angleRad < -PI)
	{
		*angleRad += TWO_PI;
	}
}

double MPUSensor::toReadableDegr(double v)
{
	v *= 57.2957795131;
	while (v < -180.0)
	{
		v += 360.0;
	}
	while (v > 180)
	{
		v -= 360;
	}
	return v;
}
#endif