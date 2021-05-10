#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <Arduino.h>

typedef void setValueFunc(String item, String value);

class Console
{
public:
    Console(setValueFunc* callback);
    void init();
    void loop();

private:
    String inputString;
    setValueFunc* callback;

    void analyse();
};

#endif
