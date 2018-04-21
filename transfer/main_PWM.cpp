#include "PWM.h"

using namespace exploringBB;

int main()
{
	PWM pwm("pwm_test_P9_22.15");
	pwm.setPeriod(10000);
	pwm.setDutyCycle(75.0f);
	pwm.setPolarity(PWM::ACTIVE_LOW);
	pwm.run();

	return 0;
}
