#include "console.h"

Console::Console(Motors *motors, Balance *balance, Position *position, structPid *balancePid, structPid *positionPid, structPid *motorPid) : motors(motors), balance(balance), position(position), balancePid(balancePid), positionPid(positionPid), motorPid(motorPid)
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
//	CONSOLE_DEBUGLN("i sending " + item + "=" + value);

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
			Serial.println("e parse error: " + inputString);
		}
		else
		{
			String item = itemAndValue.substring(0, index);
			String value = itemAndValue.substring(index + 1);
			this->handleValue(item, value);
		}
	}
	else
	{
		Serial.println("e command unknown: " + inputString);
	}
	// clear the string:
	inputString = "";
}

void Console::handleValue(String item, String value)
{

	CONSOLE_DEBUGLN("i received item=" + item + ", value=" + value);
	if (item.equals("reset"))
	{
		this->motors->reset();
		this->balance->reset();
		this->position->reset();
	}
	else if (item.equalsIgnoreCase("balanceP"))
	{
		this->balancePid->p = value.toFloat();
	}
	else if (item.equalsIgnoreCase("balanceI"))
	{
		this->balancePid->i = value.toFloat();
	}
	else if (item.equalsIgnoreCase("balanceD"))
	{
		this->balancePid->d = value.toFloat();
	}
	else if (item.equalsIgnoreCase("positionP"))
	{
		this->positionPid->p = value.toFloat();
	}
	else if (item.equalsIgnoreCase("positionI"))
	{
		this->positionPid->i = value.toFloat();
	}
	else if (item.equalsIgnoreCase("positionD"))
	{
		this->positionPid->d = value.toFloat();
	}
	else if (item.equalsIgnoreCase("motorP"))
	{
		this->motorPid->p = value.toFloat();
	}
	else if (item.equalsIgnoreCase("motorI"))
	{
		this->motorPid->i = value.toFloat();
	}
	else if (item.equalsIgnoreCase("motorD"))
	{
		this->motorPid->d = value.toFloat();
	}
	else if (item.equals("test"))
	{
		Serial.println("test is working, its value is " + value);
	}
	else if (item.equalsIgnoreCase("angularVelocity"))
	{
		this->motors->setAngularSpeed(value.toFloat());
		this->motors->setSpeed(0, 0);
	}
	else if (item.equals("enabled"))
	{
		this->balance->toggleEnable();
		this->position->toggleEnable();
	}
	else if (item.equals("motor"))
	{
		this->balance->toggleEnable();
		this->position->toggleEnable();
	}
	else
	{
		Serial.println("e Unknown item " + item + " and value " + value);
	}



	// else if (inputString.startsWith("l"))
	// {
	// 	this->balance->setMaxIntegral(this->getFloatValue());
	// }
	// else if (inputString.startsWith("o"))
	// {
	// 	this->balance->setMaxOutput(this->getFloatValue());
	// }
}
