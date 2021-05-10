#ifndef _PID_H
#define _PID_H

#include <types.h>

#define DEFAULT_PID_LIMIT 255

#define PID_DELAY_MS            10
#define PID_DELAY_S                     0.01
#define PID_DELAY_S_REVERSE     100

class PID {
public:
PID(double* input, double* output, double* target, structPid*);

void loop(unsigned long nowMs, double dtS);

void reset();

void setLimitOutput(double limit);
void setLimitIntegral(double limit);

void enableDebug();

private:
structPid* pid;

double *input;
double *output;
double *target;

unsigned long nextTime;

double integral;
double lastError;

double limitOutput;
double limitIntegral;
bool debug;

void constraint(double*, double, String);
};
#endif
