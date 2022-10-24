#include <em_gpio.h>

static const GPIO_Port_TypeDef relayPort = gpioPortC;
static const unsigned int relayPinNumber = 3;

void initializeLightControl(void) {
	GPIO_PinModeSet(relayPort, relayPinNumber, gpioModePushPull, 0);
	GPIO_PinOutSet(relayPort, relayPinNumber);
}

void lightsOn(void) {
	GPIO_PinOutClear(relayPort, relayPinNumber);
}

void lightsOff(void) {
	GPIO_PinOutSet(relayPort, relayPinNumber);
}
