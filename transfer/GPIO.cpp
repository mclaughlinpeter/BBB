#include "GPIO.h"
#include "util.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cstdlib>
#include<cstdio>
#include<fcntl.h>
#include<unistd.h>
#include<sys/epoll.h>
#include<pthread.h>

using namespace std;

namespace exploringBB {
    GPIO::GPIO(int number)
    {
        this->number = number;
        this->debounceTime = 0;
        this->togglePeriod = 100;
        this->toggleNumber = -1;
        this->callbackFunction = NULL;
        this->threadRunning = false;

        ostringstream s;
        s << "gpio" << number;
        this->name = string(s.str());
        this->path = GPIO_PATH + this->name + "/";
        this->exportGPIO();
        // need to give Linux time to setup the sysfs structure
        usleep(250000); // 250ms delay
    }

    /* int GPIO::write(string path, string filename, string value)
    {
        ofstream fs;
        fs.open((path + filename).c_str());
        if (!fs.is_open())
        {
            perror("GPIO: write failed to open file");
            return -1;
        }
        fs << value;
        fs.close();
        return 0;
    } */

    /* string GPIO::read(string path, string filename)
    {
        ifstream fs;
        fs.open((path + filename).c_str());
        if (!fs.is_open())
        {
            perror("GPIO: read failed to open file");
        }
        string input;
        getline(fs, input);
        fs.close();
        return input;
    } */

    /* int GPIO::write(string path, string filename, int value)
    {
        stringstream s;
        s << value;
        return write(path, filename, s.str());
    } */

    // private
    int GPIO::exportGPIO()
    {
        return write(GPIO_PATH, "export", this->number);
    }

    // private
    int GPIO::unexportGPIO()
    {
        return write(GPIO_PATH, "unexport", this->number);
    }

    int GPIO::setDirection(GPIO::DIRECTION dir)
    {
        switch(dir)
        {
            case INPUT: 
                return write(this->path, "direction", "in");
                break;
            case OUTPUT:
                return write(this->path, "direction", "out");
                break;
        }
        return -1;
    }

    int GPIO::setValue(GPIO::VALUE value)
    {
        switch(value)
        {
            case HIGH: 
                return write(this->path, "value", "1");
                break;
            case LOW: 
                return write(this->path, "value", "0");
                break;
        }
        return -1;
    }

    int GPIO::setEdgeType(GPIO::EDGE value)
    {
        switch(value)
        {
            case NONE: 
                return write(this->path, "edge", "none");
                break;
            case RISING: 
                return write(this->path, "edge", "rising");
                break;
            case FALLING: 
                return write(this->path, "edge", "falling");
                break;
            case BOTH: 
                return write(this->path, "edge", "both");
                break;
        }
        return -1;
    }

    int GPIO::setActiveLow(bool isLow)
    {
        if (isLow)
            return write(this->path, "active_low", "1");
        else    
            return write(this->path, "active_low", "0");
    }

    int GPIO::setActiveHigh()
    {
        return this->setActiveLow(false);
    }

    GPIO::VALUE GPIO::getValue()
    {
        string input = read(this->path, "value");   // from util class
        if ("0" == input)
            return LOW;
        else    
            return HIGH;
    }

    GPIO::DIRECTION GPIO::getDirection()
    {
        string input = read(this->path, "direction");   // from util class
        if ("in" == input)
            return INPUT;
        else    
            return OUTPUT;
    }

    GPIO::EDGE GPIO::getEdgeType()
    {
        string input = read(this->path, "edge");   // from util class
        if ("rising" == input)
            return RISING;
        else if ("falling" == input)
            return FALLING;
        else if ("both" == input)
            return BOTH;
        else    
            return NONE;
    }

    int GPIO::streamOpen()
    {
        stream.open((path + "value").c_str());
        return 0;
    }

    int GPIO::streamWrite(GPIO::VALUE value)
    {
        stream << value << std::flush;
        return 0;
    }

    int GPIO::streamClose()
    {
        stream.close();
        return 0;
    }

    int GPIO::toggleOutput()
    {
        this->setDirection(OUTPUT);
        if ((bool) this->getValue())
            this->setValue(LOW);
        else    
            this->setValue(HIGH);
        return 0;
    }

    int GPIO::toggleOutput(int time)
    {
        return this->toggleOutput(-1, time);
    }

    // numberOfTimes is how many toggles, time is togglePeriod
    int GPIO::toggleOutput(int numberOfTimes, int time)
    {
        this->setDirection(OUTPUT);
        this->toggleNumber = numberOfTimes;
        this->togglePeriod = time;
        this->threadRunning = true;

        // create thread, put handle into this->thread, invoke threadedToggle() and pass GPIO object to threadedToggle()
        // returns 0 back to caller when thread successfully created
        if (pthread_create(&this->thread, NULL, &threadedToggle, static_cast<void*>(this)))
        {
            perror("GPIO: Failed to create the toggle thread");
            this->threadRunning = false;
            return -1;
        }
        return 0;
    }

    //  friend function
    //  completes and returns when toggleNumber reaches zero
    void * threadedToggle(void * value)
    {
        GPIO * gpio = static_cast<GPIO*>(value);    // cast back to GPIO object
        bool isHigh = (bool) gpio->getValue();
        while (gpio->threadRunning)
        {
            if (isHigh) gpio->setValue(GPIO::HIGH);
            else gpio->setValue(GPIO::LOW);
            usleep(gpio->togglePeriod * 500);
            isHigh = !isHigh;
            if (gpio->toggleNumber > 0) gpio->toggleNumber--;
            if (gpio->toggleNumber == 0) gpio->threadRunning = false;
        }
        return 0;
    }

    // blocking poll
    int GPIO::waitForEdge()
    {
        this->setDirection(INPUT);
        int fd;
        int i;
        int epollfd;
        int count = 0;
        struct epoll_event ev;
        epollfd = epoll_create(1);  // creates a new epoll instance and returns a file descriptor referring to the new instance
        if (epollfd == -1)
        {
            perror("GPIO: Failed to create epollfd");
            return -1;
        }

        // open() function from fcntl.h, establishes connection between file and file descriptor
        // the file descriptor is used by other I/O functions to refer to that file
        if ((fd = open((this->path + "value").c_str(), O_RDONLY | O_NONBLOCK)) == -1)
        {
            perror("GPIO: Failed to open file");
            return -1;
        }

        // ev.events = read operation | edge triggered | urgent data
        ev.events = EPOLLIN | EPOLLET | EPOLLPRI;
        ev.data.fd = fd; // attach the file descriptor

        // register the file descriptor fd on the epoll instance epollfd and associate event ev with fd
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
        {
            perror("GPIO: Failed to add control interface");
            return -1;
        }

        while (count <= 1) // ignore the first trigger
        {
            // waits for events on file descriptor epollfd, returns number of events
            i = epoll_wait(epollfd, &ev, 1, -1);
            if (i == -1)
            {
                perror("GPIO: Poll wait fail");
                count = 5; // terminate loop
            }
            else
            {
                count++; // count the triggers
            }
        }
        close(fd);
        if (count == 5)
            return -1;
        return 0;
    }

    void * threadedPoll(void * value)
    {
        GPIO * gpio = static_cast<GPIO*>(value); // cast to pointer to GPIO object
        while (gpio->threadRunning)
        {
            gpio->callbackFunction(gpio->waitForEdge());
            usleep(gpio->debounceTime * 1000);
        }
        return 0;
    }

    int GPIO::waitForEdge(CallbackType callback)
    {
        this->threadRunning = true;
        this->callbackFunction = callback;

        if (pthread_create(&this->thread, NULL, &threadedPoll, static_cast<void*>(this)))
        {
            perror("GPIO: Failed to create the poll thread");
            this->threadRunning = false;
            return -1;
        }
        return 0;
    }

    GPIO::~GPIO()
    {
        this->unexportGPIO();
    }

}   // namespce exploring BB