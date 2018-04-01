#include<iostream>
#include<unistd.h>
#include "GPIO.h"

using namespace exploringBB;
using namespace std;

int main()
{
    GPIO outGPIO(49);
    GPIO inGPIO(115);

    outGPIO.setDirection(GPIO::OUTPUT);
    for (int i = 0; i < 10; i++)
    {
        outGPIO.setValue(GPIO::HIGH);
        usleep(500000);
        outGPIO.setValue(GPIO::LOW);
        usleep(500000);
    }
    
    return 0;
}