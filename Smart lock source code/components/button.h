#ifndef SMARTLOCK_COMPONENTS_BUTTON_H
#define SMARTLOCK_COMPONENTS_BUTTON_H

#include <stdbool.h>

// Functions for reading the state of the onboard button.

void initializeButton(void);
bool getButtonState(void);

#endif // SMARTLOCK_COMPONENTS_BUTTON_H
