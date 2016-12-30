#include "motors.h"

Motors::Motors(structMotorConfig& c1, structMotorConfig& c2, structMotorConfig& c3, structPid* pid) :
	m1(SingleMotor(c1, pid)), m2(SingleMotor(c2, pid)), m3(SingleMotor(c3, pid)){
	this->motors[0] = &m1;
	this->motors[1] = &m2;
	this->motors[2] = &m3;
}

void Motors::init(){
	for(int i=0; i<3; i++) {
		this->motors[i]->init();
	}
	MOTORS_DEBUGLN("Motors initilized");
}

void Motors::loop(unsigned long now, double dtS){
	//update motors
	for(int i=0; i<3; i++) {
		this->motors[i]->loop(now, dtS);
	}
}

void Motors::updateEncoderMotor1(uint32_t value){
	this->m1.updateEncoder(value);
}

void Motors::updateEncoderMotor2(uint32_t value){
	this->m2.updateEncoder(value);
}

void Motors::updateEncoderMotor3(uint32_t value){
	this->m3.updateEncoder(value);
}

void Motors::setSpeed(float x, float y){
	//TODO take x,y and omni
	for(int i=0; i<3; i++) {
		this->motors[i]->setSpeed(x);
	}
}
