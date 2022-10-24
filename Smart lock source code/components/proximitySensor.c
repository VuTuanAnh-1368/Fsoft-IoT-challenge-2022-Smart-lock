#include <stdbool.h>
#include <stdint.h>

#include <em_common.h>
#include <em_gpio.h>

static GPIO_Port_TypeDef proximitySensorPort = gpioPortB;
static unsigned int proximitySensorPinNumber = 3;

void initializeProximitySensor(void) {
	GPIO_PinModeSet(proximitySensorPort, proximitySensorPinNumber, gpioModeInput, 0);
}

bool getProximitySensorState(void) {
	return !GPIO_PinInGet(proximitySensorPort, proximitySensorPinNumber);
}
