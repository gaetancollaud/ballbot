#include "motors.h"

Motors::Motors(structMotorConfig& c1, structMotorConfig& c2, structMotorConfig& c3, structPid* pid) :
	m1(SingleMotor(c1, pid)), m2(SingleMotor(c2, pid)), m3(SingleMotor(c3, pid)){
	this->motors[0] = &m1;
	this->motors[1] = &m2;
	this->motors[2] = &m3;
	this->holoAngle[0] = c1.angleRad;
	this->holoAngle[1] = c2.angleRad;
	this->holoAngle[2] = c3.angleRad;
	this->angularSpeed = 0;
	this->positionX = 0;
	this->positionY = 0;
	this->speedX = 0;
	this->speedY = 0;
}

void Motors::init(){
	for(int i=0; i<3; i++) {
		this->motors[i]->init();
	}
	MOTORS_DEBUGLN("Motors initilized");
}
void Motors::reset(){
	this->positionX = 0;
	this->positionY = 0;
	this->speedX = 0;
	this->speedY = 0;
	for(int i=0; i<3; i++) {
		this->motors[i]->reset();
	}
	MOTORS_DEBUGLN("Motors reset");
}

void Motors::loop(unsigned long now, double dtS){
	this->positionX += this->speedX*dtS;
	this->positionY += this->speedY*dtS;

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

void Motors::setSpeed(double dx, double dy){

	double vectorSize = constrain(sqrt(dx * dx + dy * dy), -100, 100);
	double vectorAngle = atan2(dy, dx) + PI_180;
	if(abs(vectorSize)>=99) {
		// Serial.println("Max vector size reached");
	}

	MOTORS_DEBUG("vect\t");
	MOTORS_DEBUG(vectorSize);
	MOTORS_DEBUG(":");
	MOTORS_DEBUGLN(vectorAngle*RAD_TO_DEGR);

	for (int i = 0; i < 3; i++) {
		this->motors[i]->setSpeed(vectorSize * sin(vectorAngle + this->holoAngle[i]) + this->angularSpeed);
	}
}

void Motors::setAngularSpeed(double v){
	this->angularSpeed = v;
}

double* Motors::getPositionXPtr(){
	return &this->positionX;
}

double* Motors::getPositionYPtr(){
	return &this->positionY;
}
