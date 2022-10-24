#ifndef SMARTLOCK_COMPONENTS_CLOCK_H
#define SMARTLOCK_COMPONENTS_CLOCK_H

#include <stdint.h>

// A steady clock source.

extern uint64_t clockOneSecond;

void initializeClock(void);
void tickClock(void);

uint64_t getCurrentTime(void);

#endif // SMARTLOCK_COMPONENTS_CLOCK_H
