#ifndef _MPU_SENSOR_H
#define _MPU_SENSOR_H

#include "Arduino.h"

#include <Wire.h>
#include <MPU6050.h>


#define DEBUG_MPU

#ifdef DEBUG_MPU
	#define MPU_DEBUG(...) Serial.print(__VA_ARGS__);
	#define MPU_DEBUGLN(...) Serial.println(__VA_ARGS__);
	#define MPU_DEBUGVAL(...) Serial.print(__VA_ARGS__);Serial.print("\t");
#else
	#define MPU_DEBUG(...)
	#define MPU_DEBUGLN(...)
	#define MPU_DEBUGVAL(...)
#endif

#define RESET_LOOP 100
#define PI 3.14159265359
#define TWO_PI 6.28318530718

#define GYRO_TRUST 0.96
#define GYRO_REVERSE_TRUST 0.04


//250degr = 4.363323129992093 rad
//2^15/250degr //sensitivity +/- 250degr for 16bits
//2^15/4.363323129992093 = 0.00013315805
#define GYRO_RATIO_250 0.00013315805
#define GYRO_RATIO_500 0.0002663161
#define GYRO_RATIO GYRO_RATIO_500

//70% of 2^15
#define ALLERT_TRESHOLD 22937

class MPUSensor{
public:
	MPUSensor(int);
	void init();
	void reset();
	void loop(unsigned long, double);

	double* getAngleXptr();
	double* getAngleYptr();

private:
	int pinCS;

	MPU6050 accelgyro;
	int16_t ax, ay, az, gx, gy, gz;
	int16_t zax, zay, zaz, zgx, zgy, zgz;

	double angleX;
	double angleY;

	void refreshSensors();
	void computeAngles(double);
	double constraintAngle(double*);
	double toReadableDegr(double);

};

#endif
