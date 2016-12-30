#include "balance.h"
Balance::Balance(MPUSensor* sensor, Motors* motors, structPid* pidBalance) : sensor(sensor), motors(motors), pidBalance(pidBalance),
	pidBalanceX(sensor->getAngleXptr(), &outputBalanceX, &targetBalanceX, pidBalance),
	pidBalanceY(sensor->getAngleYptr(), &outputBalanceY, &targetBalanceY, pidBalance)
{
	this->targetBalanceX = 0;
	this->nextTime = 0;
	this->enable = false;
}

void Balance::init(){

}

void Balance::toggleEnable(){
	this->enable = !this->enable;
	if(this->enable) {
		this->reset();
		this->motors->reset();
	}else{
		this->motors->setSpeed(0,0);
	}
}

void Balance::reset(){
	this->pidBalanceX.reset();
	this->pidBalanceY.reset();
}

void Balance::loop(unsigned long nowMs, double dtS){
	if (nowMs >= this->nextTime) {
		this->nextTime = nowMs + BALANCE_DELAY_MS;

		if(this->enable) {

			pidBalanceX.loop(nowMs, BALANCE_DELAY_S);
			pidBalanceY.loop(nowMs, BALANCE_DELAY_S);

			this->motors->setSpeed(-this->outputBalanceX, this->outputBalanceY);
		}
	}
}