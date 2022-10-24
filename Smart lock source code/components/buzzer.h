#ifndef SMARTLOCK_COMPONENTS_BUZZER_H
#define SMARTLOCK_COMPONENTS_BUZZER_H

// Functions for controlling the buzzer module.

#include <stdint.h>

extern uint32_t buzzerOneSecond;

void initializeBuzzerControl(void);
void tickBuzzerControl(void);
/*
	`duration` and `gap` are in timer ticks, every 1024 ticks of the hardware clock.
	The number of ticks for one second is precomputed as `buzzerOneSecond`.
*/
void buzzerPlay(const uint32_t duration, const uint32_t gap, const uint32_t count);
void buzzerStop(void);

#endif // SMARTLOCK_COMPONENTS_BUZZER_H
