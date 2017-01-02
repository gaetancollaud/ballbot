#include "position.h"

Position::Position(Balance* balance, Motors* motors,  structPid* pidPosition) : balance(balance), motors(motors), pidPosition(pidPosition),
	pidPositionX(motors->getPositionXPtr(), &outputPositionX, &targetPositionX, pidPosition),
	pidPositionY(motors->getPositionYPtr(), &outputPositionY, &targetPositionY, pidPosition)
{
	this->outputPositionX = 0;
	this->outputPositionY = 0;
	this->nextTime = 0;

	double maxOutput = 0.0523599; // 3 degr in radan
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
		this->nextTime = nowMs + POSITION_DELAY_MS;

		if(this->enable) {

			pidPositionX.loop(nowMs, POSITION_DELAY_S);
			pidPositionY.loop(nowMs, POSITION_DELAY_S);

			POSITION_DEBUG("P:\t");
			POSITION_DEBUGVAL(*this->motors->getPositionXPtr());
			POSITION_DEBUGVAL(*this->motors->getPositionXPtr());
			POSITION_DEBUG("A:\t");
			POSITION_DEBUGVAL(this->outputPositionX);
			POSITION_DEBUGVAL(this->outputPositionY);
			POSITION_DEBUGLN();

			this->balance->setTargetAngle(this->outputPositionX, this->outputPositionY);
		}
	}
}
