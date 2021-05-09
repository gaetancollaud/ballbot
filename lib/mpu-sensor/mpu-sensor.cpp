#include "mpu-sensor.h"

MPUSensor::MPUSensor(int pinCS, int pinInterupt) : pinCS(pinCS), pinInterupt(pinInterupt)
{
	// this->zax = 0;
	// this->zay = 0;
	// this->zaz = 0;
	// this->zgx = 0;
	// this->zgy = 0;
	// this->zgz = 0;
}

void MPUSensor::init()
{
	pinMode(pinCS, OUTPUT);
	digitalWrite(pinCS, LOW);
	delay(100);
	digitalWrite(pinCS, HIGH);

	//TODO try to speed up i2c to 400khz

	Wire.begin();
	accelgyro.initialize();
	int retryLeft = 5;
	while (!accelgyro.testConnection() && retryLeft-- > 0)
	{
		Serial.println("Cannot connect to MPU6050");
		delay(1000);
	}

	accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_500);
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

	int16_t ax, ay, az;
	int16_t gx, gy, gz;

	// read raw accel/gyro measurements from device
	accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

	// Serial.print("a/g:\t");
	// Serial.print(ax);
	// Serial.print("\t");
	// Serial.print(ay);
	// Serial.print("\t");
	// Serial.print(az);
	// Serial.print("\t");
	// Serial.print(gx);
	// Serial.print("\t");
	// Serial.print(gy);
	// Serial.print("\t");
	// Serial.println(gz);
}
