#include <stdbool.h>

#include <em_gpio.h>
#include <sl_pwm.h>

#include "components/buzzer.h"
#include "components/oled.h"

static const GPIO_Port_TypeDef
	servoPort = gpioPortB,
	statusLEDPort = gpioPortA;
static const unsigned int
	servoPinNumber = 4,
	statusLEDPinNumber = 4;
static sl_pwm_instance_t servoOscillator;

static bool lockState = false;

void initializeLockControl(void) {
	GPIO_PinModeSet(servoPort, servoPinNumber, gpioModePushPull, 0);
	servoOscillator = (sl_pwm_instance_t){
		.timer = TIMER0,
		.channel = 0,
		.port = servoPort,
		.pin = servoPinNumber,
		.location = 0
	};
	sl_pwm_config_t servoConfig = {
		.frequency = 50,
		.polarity = PWM_ACTIVE_HIGH,
	};
	sl_pwm_init(&servoOscillator, &servoConfig);
	sl_pwm_set_duty_cycle(&servoOscillator, 3);
	sl_pwm_start(&servoOscillator);

	GPIO_PinModeSet(statusLEDPort, statusLEDPinNumber, gpioModePushPull, 0);
	GPIO_PinOutClear(statusLEDPort, statusLEDPinNumber);
	
	oled_drawTextSingleLine(0, 3, "Door LOCKED.");
}

void setLockState(const bool open) {
	if (lockState == open) return;
	lockState = open;
	sl_pwm_set_duty_cycle(&servoOscillator, open ? 12 : 3);
	if (open) {
		GPIO_PinOutSet(statusLEDPort, statusLEDPinNumber);
		buzzerPlay(buzzerOneSecond/20, buzzerOneSecond/20, 2);
		oled_clearRow(3);
		oled_drawTextSingleLine(0, 3, "Door OPEN.");
	} else {
		GPIO_PinOutClear(statusLEDPort, statusLEDPinNumber);
		buzzerPlay(buzzerOneSecond/20, buzzerOneSecond/20, 3);
		oled_clearRow(3);
		oled_drawTextSingleLine(0, 3, "Door LOCKED.");
	}
}

bool getLockState(void) {
	return lockState;
}
