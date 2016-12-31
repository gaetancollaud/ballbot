#include "console.h"

Console::Console(Motors* motors, Balance* balance, structPid* motorPid) : motors(motors), balance(balance), motorPid(motorPid){

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
		this->motorPid->p = this->getFloatValue();
	}else if(inputString.startsWith("i")) {
		this->motorPid->i = this->getFloatValue();
	}else if(inputString.startsWith("d")) {
		this->motorPid->d = this->getFloatValue();
	}else if(inputString.startsWith("s")) {
		this->readSpeed();
	}else if(inputString.startsWith("r")) {
		this->motors->reset();
		this->balance->reset();
	}else if(inputString.startsWith("e")) {
		this->balance->toggleEnable();
	}else if(inputString.startsWith("l")) {
		this->balance->setMaxIntegral(this->getFloatValue());
	}else if(inputString.startsWith("o")) {
		this->balance->setMaxOutput(this->getFloatValue());
	}else{
		Serial.print("Error command unknown");
	}
	// clear the string:
	inputString = "";
}

void Console::readSpeed(){
	float s = this->getFloatValue();
	Serial.print("Set speed to \t");
	Serial.println(s);
	this->motors->setSpeed(0, s);
}

float Console::getFloatValue(){
	return inputString.substring(1).toFloat();
}
