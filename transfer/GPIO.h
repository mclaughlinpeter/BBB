#ifndef GPIO_H_
#define GPIO_H_
#include<string>
#include<fstream>

using std::string;
using std::ofstream;

#define GPIO_PATH "/sys/class/gpio/"

namespace exploringBB {

    typedef int (*CallbackType)(int);   // CallbackType is a pointer to a function returning int and taking an int parameter
    
    enum GPIO_DIRECTION { INPUT, OUTPUT };
    enum GPIO_VALUE { LOW = 0, HIGH = 1 };
    enum GPIO_EDGE { NONE, RISING, FALLING, BOTH };

    class GPIO {
        private:
            int number; // pin number, ctor argument
            int debounceTime;
            string name;    //  "gpio" + number
            string path;    //  full path to pin files
        public:
            GPIO(int number);
            virtual int getNumber() { return number; }

            //  General I/O settings
            virtual int setDirection(GPIO_DIRECTION);
            virtual GPIO_DIRECTION getDirection();
            virtual int setValue(GPIO_VALUE);
            virtual GPIO_VALUE getValue();
            virtual int toggleOutput();
            virtual int setActiveLow(bool isLow = true);    // low = 1, high = 0
            virtual int setActiveHigh();    // default
            virtual void setDebounceTime(int time) { this-> debounceTime = time; }

            //  Advanced output
            virtual int streamOpen();
            virtual int streamWrite(GPIO_VALUE);
            virtual int streamClose();

            virtual int toggleOutput(int time);
            virtual int toggleOutput(int numberOfTimes, int time);
            virtual void changeToggleTime(int time) { this->togglePeriod = time; }
            virtual void toggleCancel() { this->threadRunning = false; }

            //  Advanced input
            virtual int setEdgeType(GPIO_EDGE);
            virtual GPIO_EDGE getEdgeType();
            virtual int waitForEdge();  // waits until button is pressed
            virtual int waitForEdge(CallbackType callback); // threaded with callback
            virtual void waitForEdgeCancel() { this->threadRunning = false; }

            virtual ~GPIO();    // dtor will unexport the pin

        private:
            int write(string path, string filename, string value);
            int write(string path, string filename, int value);
            string read(string path, string filename);
            int exportGPIO();
            int unexportGPIO();
            ofstream stream;
            pthread_t thread;
            CallbackType callbackFunction;
            bool threadRunning;
            int togglePeriod;   // default 100ms
            int toggleNumber;   // default -1 (infinite)
            friend void * threadedPoll(void * value);
            friend void * threadedToggle(void * value);
    };

    void * threadedPoll(void * value);
    void * threadedToggle(void * value);

}   // namespace exploringBB

#endif  // GPIO_H_