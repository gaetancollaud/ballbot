#ifndef _MPU_SENSOR_H
#define _MPU_SENSOR_H

#include "Arduino.h"

#include <Wire.h>
// #include <I2Cdev.h>
#include <MPU6050.h>
// #include <MPU6050_6Axis_MotionApps20.h>

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
#define OUTPUT_READABLE_YAWPITCHROLL

#define DEBUG_MPU

#ifdef DEBUG_MPU
#define MPU_DEBUG(...) Serial.print(__VA_ARGS__);
#define MPU_DEBUGLN(...) Serial.println(__VA_ARGS__);
#define MPU_DEBUGVAL(...)      \
	Serial.print(__VA_ARGS__); \
	Serial.print("\t");
#else
#define MPU_DEBUG(...)
#define MPU_DEBUGLN(...)
#define MPU_DEBUGVAL(...)
#endif

class MPUSensor
{
public:
	MPUSensor(int, int);
	void init();
	void reset();
	void loop(unsigned long, double);

	double *getAngleXptr();
	double *getAngleYptr();

	void detectDevices();

private:
	int pinCS;
	int pinInterupt;

	MPU6050 accelgyro;
	// int16_t ax, ay, az, gx, gy, gz;
	// int16_t zax, zay, zaz, zgx, zgy, zgz;

	// int slidingAverageIndex;
	// double slidingAverageX[AVERAGE_SLIDE];
	// double slidingAverageY[AVERAGE_SLIDE];
	double angleX;
	double angleY;

	void refreshSensors();
	// void computeAngles(double);
	// double constraintAngle(double*);
	// double toReadableDegr(double);
};

#endif
