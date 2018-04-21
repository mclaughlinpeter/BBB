#ifndef PWM_H_
#define PWM_H_

#include<string>
using std::string;

#define PWM_PATH "/sys/devices/ocp.3/"
#define PWM_PERIOD "period"
#define PWM_DUTY "duty"
#define PWM_POLARITY "polarity"
#define PWM_RUN "run"

namespace exploringBB {
    class PWM {
    public:
        enum POLARITY { ACTIVE_LOW = 0, ACTIVE_HIGH = 1 };
    private:
        string name, path;
        float analogFrequency;
        float analogMax;
    public:
        PWM(string pinName);

        virtual int setPeriod(unsigned int period_ns);
        virtual unsigned int getPeriod();
        virtual int setFrequency(float frequency_hz);
        virtual float getFrequency();
        virtual int setDutyCycle(unsigned int duration_ns);
        virtual int setDutyCycle(float percentage);
        virtual unsigned int getDutyCycle();
        virtual float getDutyCyclePercent();

        virtual int setPolarity(PWM::POLARITY);
        virtual void invertPolarity();
        virtual PWM::POLARITY getPolarity();

        virtual void setAnalogFrequency(float frequency_hz) { this->analogFrequency = frequency_hz; }
        virtual int calibrateAnalogMax(float analogMax);
        virtual int analogWrite(float voltage);

        virtual int run();
        virtual bool isRunning();
        virtual int stop();

        virtual ~PWM();
    private:
        float period_nsToFrequency(unsigned int);
        unsigned int frequencyToPeriod_ns(float);
    };
}   // namespace exploringBB

#endif // PWM_H_