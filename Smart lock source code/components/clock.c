#include <stdint.h>

#include <em_cmu.h>
#include <em_timer.h>

uint64_t clockOneSecond = 1000;

static uint64_t clockRollovers = 0;
static uint32_t
	clockTop = 1000,
	oldClockValue = 0;

void initializeClock(void) {
	CMU_ClockEnable(cmuClock_TIMER3, true);
	clockOneSecond = CMU_ClockFreqGet(cmuClock_EM01GRPACLK) / 1024;
	TIMER_Init_TypeDef timerInitParams = {
		.enable = false,
		.debugRun = true,
		.prescale = timerPrescale1024,
		.clkSel = timerClkSelHFPerClk,
		.count2x = false,
		.ati = false,
		.fallAction = timerInputActionNone,
		.riseAction = timerInputActionNone,
		.mode = timerModeUp,
		.dmaClrAct = false,
		.quadModeX4 = false,
		.oneShot = false,
		.sync = false
	};
	TIMER_Init(TIMER3, &timerInitParams);
	clockTop = TIMER_TopGet(TIMER3);
	TIMER_CounterSet(TIMER3, 0);
	TIMER_Enable(TIMER3, true);
}

void tickClock(void) {
	const uint32_t clockValue = TIMER_CounterGet(TIMER3);
	if (clockValue < oldClockValue)
		clockRollovers++;
	oldClockValue = clockValue;
}

uint64_t getCurrentTime(void) {
	return clockRollovers*clockTop + oldClockValue;
}
