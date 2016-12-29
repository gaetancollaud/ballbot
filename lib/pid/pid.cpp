#include "pid.h"

PID::PID(double* input, double* output, double* target, structPid* pid)
: input(input), output(output), target(target), pid(pid) {
	this->reset();
	this->limit = DEFAULT_PID_LIMIT;
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
		double i = this->pid->i;//* PID_DELAY_S_REVERSE;
		double d = this->pid->d;// * PID_DELAY_S;

		// this->nextTime = nowMs + PID_DELAY_MS;
		double input = *this->input;
		double error = *this->target - input;

		//proportional
		double proportional = p * error;

		//integral
		this->integral += (error * dtS);
		// this->constraint(&this->integral);

		//derivative
		double derivative = d * ((error - this->lastError) / dtS);

		double output = proportional + i * this->integral + derivative;
		this->constraint(&output);

		*this->output = output;

		this->lastError = error;
	// }
}

void PID::setLimit(double limit){
	this->limit = limit;
}

void PID::constraint(double* v) {
	double lim = this->limit;
	if (*v > lim) {
		*v = lim;
	} else if (*v < -lim) {
		*v = -lim;
	}
}
