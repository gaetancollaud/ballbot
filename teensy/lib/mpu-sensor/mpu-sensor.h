#ifndef _MPU_SENSOR_DMP_H
#define _MPU_SENSOR_DMP_H

#include "Arduino.h"

// #define MODE_DMP

#include <I2Cdev.h>
#include <Wire.h>
#include <helper_3dmath.h>

#ifndef MODE_DMP
#include <MPU6050.h>
#endif

#ifndef MODE_DMP
#define GYRO_TRUST 0.999
#define GYRO_REVERSE_TRUST (1.0 - GYRO_TRUST)
#define GYRO_RATIO_250 0.00013315805
#define GYRO_RATIO_500 0.0002663161
#define GYRO_RATIO GYRO_RATIO_500
#define ALLERT_TRESHOLD 22937
#define RESET_LOOP 100
#define AVERAGE_SLIDE 2
#define AVERAGE_SLIDE_REVERSE (1.0 / AVERAGE_SLIDE)
#endif

// #define DEBUG_MPU

#ifdef DEBUG_MPU
#define MPU_DEBUG(...) Serial.print(__VA_ARGS__);
#define MPU_DEBUGLN(...) Serial.println(__VA_ARGS__);
#define MPU_DEBUGVAL(...)      \
	Serial.print(__VA_ARGS__); \
	Serial.print("\t");
#define MPU_DEBUGF(...)           \
	char buffer[100];             \
	sprintf(buffer, __VA_ARGS__); \
	Serial.print(buffer);
#else
#define MPU_DEBUG(...)
#define MPU_DEBUGLN(...)
#define MPU_DEBUGVAL(...)
#define MPU_DEBUGF(...)
#endif

typedef void (*mpuInterrupFunction)(void);

class MPU6050;

class MPUSensor
{
public:
	MPUSensor(int);
	void init(mpuInterrupFunction func);
	void reset();
	void loop(unsigned long, double);

	double *getAngleXptr();
	double *getAngleYptr();
	double *getAngleZptr();

	void detectDevices();
	void dmpDataReady();

private:
	int pinInterupt;

	MPU6050 *accelgyro;

	double angleX;
	double angleY;
	double angleZ;

	void refreshSensors();
	void computeAngles(double);

#ifndef MODE_DMP
	int16_t ax, ay, az, gx, gy, gz;
	int16_t zax, zay, zaz, zgx, zgy, zgz;
	int slidingAverageIndex;
	double slidingAverageX[AVERAGE_SLIDE];
	double slidingAverageY[AVERAGE_SLIDE];

	double constraintAngle(double *);
	double toReadableDegr(double);
#endif

#ifdef MODE_DMP

	volatile bool mpuInterrupt = false;

	bool dmpReady = false;	// set true if DMP init was successful
	uint8_t mpuIntStatus;	// holds actual interrupt status byte from MPU
	uint8_t devStatus;		// return status after each device operation (0 = success, !0 = error)
	uint16_t packetSize;	// expected DMP packet size (default is 42 bytes)
	uint16_t fifoCount;		// count of all bytes currently in FIFO
	uint8_t fifoBuffer[64]; // FIFO storage buffer
	// orientation/motion vars
	Quaternion q;		 // [w, x, y, z]         quaternion container
	VectorFloat gravity; // [x, y, z]            gravity vector
	float euler[3];		 // [psi, theta, phi]    Euler angle container
	float ypr[3];		 // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

#endif
};

#endif