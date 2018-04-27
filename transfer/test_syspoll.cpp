#include<iostream>
#include "GPIO.h"
#include <unistd.h>

using namespace exploringBB;
using namespace std;

int main()
{
    if(getuid() != 0)
    {
        cout << "You must run this program as root. Exiting." << endl;
        return -1;
    }

    GPIO outGPIO(49);
    GPIO inGPIO(115);

    inGPIO.setDirection(INPUT);
    outGPIO.setDirection(OUTPUT);
    inGPIO.setEdgeType(RISING);
    outGPIO.streamOpen();
    outGPIO.streamWrite(LOW);

    cout << "Press the button:" << endl;
    inGPIO.waitForEdge();
    outGPIO.streamWrite(HIGH);
    usleep(2000000);
    outGPIO.streamClose();

    return 0;
}