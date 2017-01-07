#include "console.h"

Console::Console(Motors* motors, Balance* balance, Position* position, structPid* balancePid, structPid* positionPid) :
	motors(motors), balance(balance), position(position), balancePid(balancePid), positionPid(positionPid){

}

void Console::init(){
	this->inputString = "";
}

void Console::loop(unsigned long, double){
	while (Serial.available()) {
		// get the new byte:
		char inChar = (char)Serial.read();
		// add it to the inputString:
		inputString += inChar;
		// if the incoming character is a newline, set a flag
		// so the main loop can do something about it:
		if (inChar == '\n') {
			this->analyse();
		}
	}
}


void Console::analyse(){
	Serial.println(inputString);
	if(inputString.startsWith("p")) {
		this->balancePid->p = this->getFloatValue();
	}else if(inputString.startsWith("i")) {
		this->balancePid->i = this->getFloatValue();
	}else if(inputString.startsWith("d")) {
		this->balancePid->d = this->getFloatValue();
	}else if(inputString.startsWith("a")) {
		this->positionPid->p = this->getFloatValue();
	}else if(inputString.startsWith("s")) {
		this->positionPid->i = this->getFloatValue();
	}else if(inputString.startsWith("d")) {
		this->positionPid->d = this->getFloatValue();
	}else if(inputString.startsWith("r")) {
		this->motors->reset();
		this->balance->reset();
		this->position->reset();
	}else if(inputString.startsWith("e")) {
		this->balance->toggleEnable();
		this->position->toggleEnable();
	}else if(inputString.startsWith("l")) {
		this->balance->setMaxIntegral(this->getFloatValue());
	}else if(inputString.startsWith("o")) {
		this->balance->setMaxOutput(this->getFloatValue());
	}else if(inputString.startsWith("v")) {
		this->motors->setAngularSpeed(this->getFloatValue());
		this->motors->setSpeed(0,0);
	}else{
		Serial.print("Error command unknown");
	}
	// clear the string:
	inputString = "";
}

float Console::getFloatValue(){
	return inputString.substring(1).toFloat();
}
