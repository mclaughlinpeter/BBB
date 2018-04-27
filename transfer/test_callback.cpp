#include<iostream>
#include "GPIO.h"
#include <unistd.h>

using namespace exploringBB;
using namespace std;

GPIO *outGPIO, *inGPIO;

int activateLED(int var)
{
    outGPIO->streamWrite(HIGH);
    cout << "Button Pressed." << endl;
    return 0;
}

int main()
{
    if(getuid() != 0)
    {
        cout << "You must run this program as root. Exiting." << endl;
        return -1;
    }

    inGPIO = new GPIO(115);
    outGPIO = new GPIO(49);

    inGPIO->setDirection(INPUT);
    outGPIO->setDirection(OUTPUT);
    inGPIO->setEdgeType(RISING);
    outGPIO->streamOpen();
    outGPIO->streamWrite(LOW);

    cout << "You have 10 seconds to press the button:" << endl;
    inGPIO->waitForEdge(&activateLED);
    cout << "Listening, but also doing something else...." << endl;

    for (int i = 0; i < 10; i++)
    {
        usleep(1000000);
        cout << "\t" << i + 1 << endl;
    }

    outGPIO->streamWrite(LOW);
    outGPIO->streamClose();

    return 0;
}