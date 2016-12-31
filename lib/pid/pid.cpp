#include "pid.h"

PID::PID(double* input, double* output, double* target, structPid* pid)
	: input(input), output(output), target(target), pid(pid) {
	this->reset();
	this->limitOutput = DEFAULT_PID_LIMIT;
	this->limitIntegral = DEFAULT_PID_LIMIT;
	this->debug = false;
}

void PID::reset() {
	this->lastError;
	this->integral = 0;
	this->nextTime = 0;
	this->lastError = 0;
}

void PID::loop(unsigned long nowMs, double dtS) {
	// if (nowMs >= this->nextTime) {
	double p = this->pid->p;
	//TODO pass dtSReverse
	double i = this->pid->i/dtS;        //* PID_DELAY_S_REVERSE;
	double d = this->pid->d*dtS;        // * PID_DELAY_S;

	// this->nextTime = nowMs + PID_DELAY_MS;
	double input = *this->input;
	double error = *this->target - input;

	//proportional
	double proportional = p * error;

	//integral
	this->integral += (error * dtS);
	this->constraint(&this->integral, this->limitIntegral, "integral");

	//derivative
	double derivative = d * ((error - this->lastError) / dtS);

	double output = proportional + i * this->integral + derivative;
	this->constraint(&output, this->limitOutput, "output");

	*this->output = output;

	this->lastError = error;
	// }
}

void PID::enableDebug(){
	this->debug = true;
}

void PID::setLimitOutput(double limit){
	this->limitOutput = limit;
}

void PID::setLimitIntegral(double limit){
	this->limitIntegral = limit;
}

void PID::constraint(double* v, double lim, String deb) {
	if (*v > lim) {
		*v = lim;
		if(this->debug) {
			Serial.print("High limit: ");
			Serial.println(deb);
		}
	}
	else if (*v < -lim) {
		*v = -lim;
		if(this->debug) {
			Serial.print("Low limit: ");
			Serial.println(deb);
		}
	}
}
