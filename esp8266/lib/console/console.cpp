#include "console.h"

Console::Console(setValueFunc callback) : callback(callback)
{
}

void Console::init()
{
	Serial.begin(115200);
	this->inputString = "";
}

void Console::loop()
{
	while (Serial.available())
	{
		// get the new byte:
		char inChar = (char)Serial.read();
		// add it to the inputString:
		inputString += inChar;
		// if the incoming character is a newline, set a flag
		// so the main loop can do something about it:
		if (inChar == '\n')
		{
			this->analyse();
		}
	}
}

void Console::analyse()
{
	if (inputString.startsWith("i"))
	{
		//ignore
	}
	else if (inputString.startsWith("s"))
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
			this->callback(item, value);
		}
	}
	else
	{
		Serial.print("e command unknown: " + inputString);
	}
	// clear the string:
	inputString = "";
}
