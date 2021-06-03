#include "balance.h"
Balance::Balance(MPUSensor* sensor, Motors* motors, structPid* pidBalance) : sensor(sensor), motors(motors), pidBalance(pidBalance),
	pidBalanceX(sensor->getAngleXptr(), &outputBalanceX, &targetBalanceX, pidBalance),
	pidBalanceY(sensor->getAngleYptr(), &outputBalanceY, &targetBalanceY, pidBalance)
{
	this->targetBalanceX = 0;
	this->targetBalanceY = 0;
	this->nextTime = 0;
	this->enable = false;

	double maxOutput = 100;
	double maxIntegral = 10000;
	pidBalanceX.setLimitOutput(maxOutput);
	pidBalanceY.setLimitOutput(maxOutput);
	pidBalanceX.setLimitIntegral(maxIntegral);
	pidBalanceY.setLimitIntegral(maxIntegral);
	pidBalanceX.enableDebug();
	pidBalanceY.enableDebug();
}

void Balance::init(){

}

void Balance::toggleEnable(){
	this->enable = !this->enable;
	if(this->enable) {
		this->reset();
	}else{
		this->motors->setSpeed(0,0);
	}
	this->motors->reset();
}

void Balance::setMaxIntegral(double v){
	pidBalanceX.setLimitIntegral(v);
	pidBalanceY.setLimitIntegral(v);
}

void Balance::setMaxOutput(double v){
	pidBalanceX.setLimitOutput(v);
	pidBalanceY.setLimitOutput(v);
}

void Balance::setTargetAngle(double x, double y){
	this->targetBalanceX = x;
	this->targetBalanceY = y;
}

void Balance::reset(){
	this->pidBalanceX.reset();
	this->pidBalanceY.reset();
	this->accDts = 0;
}

void Balance::loop(unsigned long nowMs, double dtS){
	this->accDts += dtS;
	if (nowMs >= this->nextTime) {
		this->nextTime = nowMs + BALANCE_DELAY_MS;

		if(this->enable) {

			pidBalanceX.loop(nowMs, accDts);
			pidBalanceY.loop(nowMs, accDts);

			this->motors->setSpeed(-this->outputBalanceX, this->outputBalanceY);
		}
		this->accDts = 0;
	}
}
