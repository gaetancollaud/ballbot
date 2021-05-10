#ifndef _MPU_SENSOR_H
#define _MPU_SENSOR_H

#include "Arduino.h"

#include <I2Cdev.h>
// #include <MPU6050.h>
#include <Wire.h>
#include <helper_3dmath.h>

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
// #define OUTPUT_READABLE_YAWPITCHROLL

// #define OUTPUT_TEAPOT

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

	void detectDevices();
	void dmpDataReady();

private:
	int pinInterupt;

	MPU6050 *accelgyro;
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

	volatile bool mpuInterrupt = false;

	bool dmpReady = false;	// set true if DMP init was successful
	uint8_t mpuIntStatus;	// holds actual interrupt status byte from MPU
	uint8_t devStatus;		// return status after each device operation (0 = success, !0 = error)
	uint16_t packetSize;	// expected DMP packet size (default is 42 bytes)
	uint16_t fifoCount;		// count of all bytes currently in FIFO
	uint8_t fifoBuffer[64]; // FIFO storage buffer
	// orientation/motion vars
	Quaternion q;		 // [w, x, y, z]         quaternion container
	VectorInt16 aa;		 // [x, y, z]            accel sensor measurements
	VectorInt16 aaReal;	 // [x, y, z]            gravity-free accel sensor measurements
	VectorInt16 aaWorld; // [x, y, z]            world-frame accel sensor measurements
	VectorFloat gravity; // [x, y, z]            gravity vector
	float euler[3];		 // [psi, theta, phi]    Euler angle container
	float ypr[3];		 // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

	// packet structure for InvenSense teapot demo
	uint8_t teapotPacket[14] = {'$', 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00, '\r', '\n'};
};

#endif
