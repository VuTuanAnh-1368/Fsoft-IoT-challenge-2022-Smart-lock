#include <stdbool.h>
#include <stdint.h>

#include <em_common.h>
#include <em_gpio.h>

static GPIO_Port_TypeDef buttonPort = gpioPortC;
static unsigned int buttonPinNumber = 7;

void initializeButton(void) {
	GPIO_PinModeSet(buttonPort, buttonPinNumber, gpioModeInput, 0);
}

bool getButtonState(void) {
	return !GPIO_PinInGet(buttonPort, buttonPinNumber);
}
