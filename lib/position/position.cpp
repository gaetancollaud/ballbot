#include "position.h"

Position::Position(Balance* balance, Motors* motors,  structPid* pidPosition) : balance(balance), motors(motors), pidPosition(pidPosition),
	pidPositionX(motors->getPositionXPtr(), &outputPositionX, &targetPositionX, pidPosition),
	pidPositionY(motors->getPositionYPtr(), &outputPositionY, &targetPositionY, pidPosition)
{
	this->outputPositionX = 0;
	this->outputPositionY = 0;
	this->nextTime = 0;

	double maxOutput = 0.0872665; // 5 degr in radan
	double maxIntegral = 0.001;
	pidPositionX.setLimitOutput(maxOutput);
	pidPositionY.setLimitOutput(maxOutput);
	pidPositionX.setLimitIntegral(maxIntegral);
	pidPositionY.setLimitIntegral(maxIntegral);
	// pidPositionX.enableDebug();
	// pidPositionY.enableDebug();
}

void Position::init(){

}

void Position::toggleEnable(){
	this->enable = !this->enable;
	if(this->enable) {
		this->reset();
	}
}

void Position::reset(){
	this->pidPositionX.reset();
	this->pidPositionY.reset();
}

void Position::loop(unsigned long nowMs, double dtS){
	if (nowMs >= this->nextTime) {
		this->nextTime = nowMs + BALANCE_DELAY_MS;

		if(this->enable) {

			pidPositionX.loop(nowMs, BALANCE_DELAY_S);
			pidPositionY.loop(nowMs, BALANCE_DELAY_S);

			// this->balance->setTargetAngle(this->outputPositionX, this->outputPositionY);
		}
	}
}
