#include <stdbool.h>
#include <stdint.h>

#include <em_cmu.h>
#include <em_gpio.h>
#include <em_timer.h>

uint32_t buzzerOneSecond = 1000;

static GPIO_Port_TypeDef buzzerPort = gpioPortB;
static unsigned int buzzerPinNumber = 0;

static uint32_t
	buzzerDuration = 100,
	buzzerGap = 100,
	buzzerPhasesLeft = 0;

void initializeBuzzerControl(void) {
	GPIO_PinModeSet(buzzerPort, buzzerPinNumber, gpioModePushPull, 0);
	CMU_ClockEnable(cmuClock_TIMER4, true);
	/*
		The frequency of the hardware clock is outside 16-bit resolution of the
		timer counter, division by 1024 is the biggest scaling available.
	*/
	buzzerOneSecond = CMU_ClockFreqGet(cmuClock_EM01GRPACLK) / 1024;
	TIMER_Init_TypeDef timerInitParams = {
		.enable = false,
		.debugRun = true,
		.prescale = timerPrescale1024,
		.clkSel = timerClkSelHFPerClk,
		.count2x = false,
		.ati = false,
		.fallAction = timerInputActionNone,
		.riseAction = timerInputActionNone,
		.mode = timerModeDown,
		.dmaClrAct = false,
		.quadModeX4 = false,
		.oneShot = true,
		.sync = false
	};
	TIMER_Init(TIMER4, &timerInitParams);
}

void tickBuzzerControl(void) {
	const uint32_t time = TIMER_CounterGet(TIMER4);
	/*
	char s[10];
	sprintf(s, "%d", (int)time);
	oled_clearScreen(0);
	oled_draw_string(5, 5, s);
	*/
	if (
		buzzerPhasesLeft == 0
		// When time is up the timer counter wraps back to the max value then stops.
		|| time != 0xFFFFu
	) return;
	if (--buzzerPhasesLeft == 0) {
		GPIO_PinOutClear(buzzerPort, buzzerPinNumber);
		return;
	}
	if (buzzerPhasesLeft % 2 == 0) {
		GPIO_PinOutClear(buzzerPort, buzzerPinNumber);
		TIMER_CounterSet(TIMER4, buzzerGap);
	} else {
		GPIO_PinOutSet(buzzerPort, buzzerPinNumber);
		TIMER_CounterSet(TIMER4, buzzerDuration);
	}
	TIMER_Enable(TIMER4, true);
}

void buzzerPlay(const uint32_t duration, const uint32_t gap, const uint32_t count) {
	buzzerDuration = duration;
	buzzerGap = gap;
	// `count` on phases plus `count-1` off phases in between.
	buzzerPhasesLeft = count*2 - 1;
	GPIO_PinOutSet(buzzerPort, buzzerPinNumber);
	TIMER_CounterSet(TIMER4, buzzerDuration);
	TIMER_Enable(TIMER4, true);
}

void buzzerStop(void) {
	buzzerPhasesLeft = 0;
	GPIO_PinOutClear(buzzerPort, buzzerPinNumber);
	TIMER_Enable(TIMER4, false);
}
