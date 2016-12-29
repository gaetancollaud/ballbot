#ifndef _PID_H
#define _PID_H

#include <types.h>

#define DEFAULT_PID_LIMIT 255

#define PID_DELAY_MS		10
#define PID_DELAY_S			0.01
#define PID_DELAY_S_REVERSE	100

class PID {
public:
	PID(double* input, double* output, double* target, structPid*);

	void loop(unsigned long nowMs, double dtS);

	void reset();

	void setLimit(double limit);

private:
	structPid* pid;

	double *input;
	double *output;
	double *target;

	unsigned long nextTime;

	double integral;
	double lastError;

	double limit;

	void constraint(double*);
};
#endif
