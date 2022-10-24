#ifndef SMARTLOCK_COMPONENTS_PROXIMITYSENSOR_H
#define SMARTLOCK_COMPONENTS_PROXIMITYSENSOR_H

// Functions of the proximity sensor.

#include <stdbool.h>
#include <stdint.h>

void initializeProximitySensor(void);
bool getProximitySensorState(void);

#endif // SMARTLOCK_COMPONENTS_PROXIMITYSENSOR_H
