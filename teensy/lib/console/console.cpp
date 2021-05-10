#include "console.h"

Console::Console(Motors *motors, Balance *balance, Position *position, structPid *balancePid, structPid *positionPid) : motors(motors), balance(balance), position(position), balancePid(balancePid), positionPid(positionPid)
{
}

void Console::init()
{
	CONSOLE_SERIAL.begin(115200);
	this->inputString = "";
}

void Console::loop(unsigned long, double)
{
	while (CONSOLE_SERIAL.available())
	{
		// get the new byte:
		char inChar = (char)CONSOLE_SERIAL.read();
		// if the incoming character is a newline, set a flag
		// so the main loop can do something about it:
		if (inChar == '\n')
		{
			if (inputString.length() > 0)
			{
				this->analyse();
			}
		}
		else if (inChar == '\r')
		{
			// ignore
		}
		else
		{
			// add it to the inputString:
			inputString += inChar;
		}
	}
}

void Console::sendItem(String item, String value)
{
	Serial.println("Sending " + item + "=" + value);

	CONSOLE_SERIAL.print("s ");
	CONSOLE_SERIAL.print(item);
	CONSOLE_SERIAL.print(" ");
	CONSOLE_SERIAL.println(value);
}

void Console::sendItem(String item, int value)
{
	char buffer[15];
	sprintf(buffer, "%d", value);
	this->sendItem(item, String(buffer));
}

void Console::sendItem(String item, double value)
{
	char buffer[15];
	sprintf(buffer, "%.5f", value);
	this->sendItem(item, String(buffer));
}

void Console::analyse()
{
	if (inputString.startsWith("i"))
	{
		//ignore
	}
	else if (inputString.startsWith("v"))
	{
		String itemAndValue = inputString.substring(2);
		int index = itemAndValue.indexOf(" ");
		if (index == -1)
		{
			Serial.print("e parse error: " + inputString);
		}
		else
		{
			String item = itemAndValue.substring(0, index);
			String value = itemAndValue.substring(index + 1);
			Serial.println("TODO handle command item=" + item + " value=" + value);
			// this->callback(item, value);
		}
	}
	else
	{
		Serial.println("e command unknown: " + inputString);
	}
	// clear the string:
	inputString = "";

	// if (inputString.startsWith("p"))
	// {
	// 	this->balancePid->p = this->getFloatValue();
	// }
	// else if (inputString.startsWith("i"))
	// {
	// 	this->balancePid->i = this->getFloatValue();
	// }
	// else if (inputString.startsWith("d"))
	// {
	// 	this->balancePid->d = this->getFloatValue();
	// }
	// else if (inputString.startsWith("a"))
	// {
	// 	this->positionPid->p = this->getFloatValue();
	// }
	// else if (inputString.startsWith("s"))
	// {
	// 	this->positionPid->i = this->getFloatValue();
	// }
	// else if (inputString.startsWith("d"))
	// {
	// 	this->positionPid->d = this->getFloatValue();
	// }
	// else if (inputString.startsWith("r"))
	// {
	// 	this->motors->reset();
	// 	this->balance->reset();
	// 	this->position->reset();
	// }
	// else if (inputString.startsWith("e"))
	// {
	// 	this->balance->toggleEnable();
	// 	this->position->toggleEnable();
	// }
	// else if (inputString.startsWith("l"))
	// {
	// 	this->balance->setMaxIntegral(this->getFloatValue());
	// }
	// else if (inputString.startsWith("o"))
	// {
	// 	this->balance->setMaxOutput(this->getFloatValue());
	// }
	// else if (inputString.startsWith("v"))
	// {
	// 	this->motors->setAngularSpeed(this->getFloatValue());
	// 	this->motors->setSpeed(0, 0);
	// }
	// else
	// {
	// 	Serial.print("Error command unknown");
	// }
	// // clear the string:
	// inputString = "";
}

float Console::getFloatValue()
{
	return inputString.substring(1).toFloat();
}
