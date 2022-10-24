#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app/app.h"

#include "components/button.h"
#include "components/buzzer.h"
#include "components/clock.h"
#include "components/light.h"
#include "components/lock.h"
#include "components/oled.h"
#include "components/proximitySensor.h"
#include "components/temperatureSensor.h"

unsigned int
	personCount = 0,
	personLimit = 4;

static uint64_t
	detectionStartTime = 0,
	temperatureStabilizationStartTime = 0,
	lastMessageTime = 0;

static bool
	firstSecondPassed = false,
	detectedEntry = false,
	buttonWasPressed = false;

void initializeDoorHandler(void) {
	oled_drawTextSingleLine(0, 0, "Persons in room:");
	char toPrint[10];
	sprintf(toPrint, "%d / %d", personCount, personLimit);
	oled_drawTextSingleLine(0, 1, toPrint);
}

void tickDoorHandler(void) {
	const bool buttonPressed = getButtonState();
	if (buttonPressed && !buttonWasPressed)
		setLockState(!getLockState());
	buttonWasPressed = buttonPressed;

	if (!getLockState()) {
		detectionStartTime = 0;
		firstSecondPassed = false;
		detectedEntry = false;
		return;
	}

	const uint64_t time = getCurrentTime();
	if (getProximitySensorState()) {
		if (detectionStartTime == 0) {
			detectionStartTime = time;
		} else {
			const uint64_t timePassed = time - detectionStartTime;
			if (timePassed >= clockOneSecond) {
				if (!firstSecondPassed) {
					buzzerPlay(buzzerOneSecond/20, 0, 1);
					firstSecondPassed = true;
				}
				const float temperature = readObjectTempC();
				if (temperature >= 35) {
					if (temperatureStabilizationStartTime == 0)
						temperatureStabilizationStartTime = time;
				} else {
					temperatureStabilizationStartTime = 0;
				}
				if (
					!detectedEntry && timePassed >= clockOneSecond*3
					&& temperatureStabilizationStartTime != 0
					&& time - temperatureStabilizationStartTime >= clockOneSecond*2
				) {
					char toPrint[10];
					if (lastMessageTime != 0) {
						oled_clearRow(6);
						oled_clearRow(7);
					}
					const int
						temperatureTenths = temperature * 10,
						temperatureWhole = temperatureTenths / 10,
						temperatureFractional = temperatureTenths % 10;
					sprintf(toPrint, "%d,%d deg", temperatureWhole, temperatureFractional);
					oled_drawTextSingleLine(0, 6, toPrint);
					if (temperature >= 38) {
						buzzerPlay(buzzerOneSecond/10*3, buzzerOneSecond/10*3, 10);
						oled_drawTextSingleLine(0, 7, "FEVER DETECTED.");
					} else if (personCount == personLimit) {
						buzzerPlay(buzzerOneSecond/10*3, buzzerOneSecond/10*3, 4);
						oled_drawTextSingleLine(0, 7, "ROOM FULL.");
					} else {
						if (personCount == 0) lightsOn();
						personCount++;
						buzzerPlay(buzzerOneSecond/2, 0, 1);
						sprintf(toPrint, "%d / %d", personCount, personLimit);
						oled_clearRow(1);
						oled_drawTextSingleLine(0, 1, toPrint);
						oled_drawTextSingleLine(0, 7, "Entry accepted.");
						updatePersonCountReport(personCount);
						sendBluetoothReportData();
					}
					lastMessageTime = time;
					detectedEntry = true;
				}
			}
		}
	} else if (detectionStartTime != 0) {
		if (time - detectionStartTime >= clockOneSecond && !detectedEntry && personCount != 0) {
			personCount--;
			if (personCount == 0) lightsOff();
			char toPrint[10];
			sprintf(toPrint, "%d / %d", personCount, personLimit);
			oled_clearRow(1);
			oled_drawTextSingleLine(0, 1, toPrint);
			updatePersonCountReport(personCount);
			sendBluetoothReportData();
		}
		detectionStartTime = 0;
		firstSecondPassed = false;
		detectedEntry = false;
	}
	if (lastMessageTime != 0 && time - lastMessageTime >= clockOneSecond*5) {
		oled_clearRow(6);
		oled_clearRow(7);
		lastMessageTime = 0;
	}
}
