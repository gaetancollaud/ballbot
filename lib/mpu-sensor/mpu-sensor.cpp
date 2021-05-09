#include "mpu-sensor.h"

#include <MPU6050_6Axis_MotionApps20.h>

MPUSensor::MPUSensor(Status* status, int pinInterupt) : status(status), pinInterupt(pinInterupt)
{
	// this->zax = 0;
	// this->zay = 0;
	// this->zaz = 0;
	// this->zgx = 0;
	// this->zgy = 0;
	// this->zgz = 0;
	this->accelgyro = new MPU6050();
}

void MPUSensor::init(mpuInterrupFunction func)
{
	// pinMode(pinCS, OUTPUT);
	// digitalWrite(pinCS, LOW);
	// delay(100);
	// digitalWrite(pinCS, HIGH);

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

	// accelgyro->setFullScaleGyroRange(MPU6050_GYRO_FS_500);
	MPU_DEBUGLN("Sensor initilized");
}

void MPUSensor::reset()
{
}

void MPUSensor::loop(unsigned long now, double dtS)
{
	this->refreshSensors();
	// this->computeAngles(dtS);
}

void MPUSensor::dmpDataReady()
{
	this->mpuInterrupt = true;
}

double *MPUSensor::getAngleXptr()
{
	return &this->angleX;
}

double *MPUSensor::getAngleYptr()
{
	return &this->angleY;
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

			status->increaseSensorCount();

#ifdef OUTPUT_READABLE_QUATERNION
			// display quaternion values in easy matrix form: w x y z
			accelgyro->dmpGetQuaternion(&q, fifoBuffer);
			Serial.print("quat\t");
			Serial.print(q.w);
			Serial.print("\t");
			Serial.print(q.x);
			Serial.print("\t");
			Serial.print(q.y);
			Serial.print("\t");
			Serial.println(q.z);
#endif

#ifdef OUTPUT_READABLE_EULER
			// display Euler angles in degrees
			accelgyro->dmpGetQuaternion(&q, fifoBuffer);
			accelgyro->dmpGetEuler(euler, &q);
			Serial.print("euler\t");
			Serial.print(euler[0] * 180 / M_PI);
			Serial.print("\t");
			Serial.print(euler[1] * 180 / M_PI);
			Serial.print("\t");
			Serial.println(euler[2] * 180 / M_PI);
#endif

#ifdef OUTPUT_READABLE_YAWPITCHROLL
			// display Euler angles in degrees
			accelgyro->dmpGetQuaternion(&q, fifoBuffer);
			accelgyro->dmpGetGravity(&gravity, &q);
			accelgyro->dmpGetYawPitchRoll(ypr, &q, &gravity);
			Serial.print("ypr\t");
			Serial.print(ypr[0] * 180 / M_PI);
			Serial.print("\t");
			Serial.print(ypr[1] * 180 / M_PI);
			Serial.print("\t");
			Serial.println(ypr[2] * 180 / M_PI);
#endif

#ifdef OUTPUT_READABLE_REALACCEL
			// display real acceleration, adjusted to remove gravity
			accelgyro->dmpGetQuaternion(&q, fifoBuffer);
			accelgyro->dmpGetAccel(&aa, fifoBuffer);
			accelgyro->dmpGetGravity(&gravity, &q);
			accelgyro->dmpGetLinearAccel(&aaReal, &aa, &gravity);
			Serial.print("areal\t");
			Serial.print(aaReal.x);
			Serial.print("\t");
			Serial.print(aaReal.y);
			Serial.print("\t");
			Serial.println(aaReal.z);
#endif

#ifdef OUTPUT_READABLE_WORLDACCEL
			// display initial world-frame acceleration, adjusted to remove gravity
			// and rotated based on known orientation from quaternion
			accelgyro->dmpGetQuaternion(&q, fifoBuffer);
			accelgyro->dmpGetAccel(&aa, fifoBuffer);
			accelgyro->dmpGetGravity(&gravity, &q);
			accelgyro->dmpGetLinearAccel(&aaReal, &aa, &gravity);
			accelgyro->dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
			Serial.print("aworld\t");
			Serial.print(aaWorld.x);
			Serial.print("\t");
			Serial.print(aaWorld.y);
			Serial.print("\t");
			Serial.println(aaWorld.z);
#endif

#ifdef OUTPUT_TEAPOT
			// display quaternion values in InvenSense Teapot demo format:
			teapotPacket[2] = fifoBuffer[0];
			teapotPacket[3] = fifoBuffer[1];
			teapotPacket[4] = fifoBuffer[4];
			teapotPacket[5] = fifoBuffer[5];
			teapotPacket[6] = fifoBuffer[8];
			teapotPacket[7] = fifoBuffer[9];
			teapotPacket[8] = fifoBuffer[12];
			teapotPacket[9] = fifoBuffer[13];
			Serial.write(teapotPacket, 14);
			teapotPacket[11]++; // packetCount, loops at 0xFF on purpose
#endif
		}
	}

	// int16_t ax, ay, az;
	// int16_t gx, gy, gz;

	// // read raw accel/gyro measurements from device
	// accelgyro->getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

	// // Serial.print("a/g:\t");
	// // Serial.print(ax);
	// // Serial.print("\t");
	// // Serial.print(ay);
	// // Serial.print("\t");
	// // Serial.print(az);
	// // Serial.print("\t");
	// // Serial.print(gx);
	// // Serial.print("\t");
	// // Serial.print(gy);
	// // Serial.print("\t");
	// // Serial.println(gz);
}
