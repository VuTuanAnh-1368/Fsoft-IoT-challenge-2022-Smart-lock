#include <stdint.h>
#include <string.h>

#include <em_cmu.h>
#include <sl_i2cspm.h>

#include "oled.h"
#include "oledDefines.h"
#include "oledFont.h"

unsigned char oled_blankBuffer[129] = {OLED_CONTROL_BYTE_DATA_STREAM};

void oled_initalizeI2CInterface() {
	CMU_ClockEnable(cmuClock_GPIO, true);
	I2CSPM_Init_TypeDef I2CInitParams = {
		.port = OLED_I2CPORT,
		.sclPort = OLED_IOPORT,
		.sclPin = OLED_SCL,
		.sdaPort = OLED_IOPORT,
		.sdaPin = OLED_SDA,
		.i2cRefFreq = 0,
		.i2cMaxFreq = I2C_FREQ_STANDARD_MAX,
		.i2cClhr = i2cClockHLRStandard
	};
	I2CSPM_Init(&I2CInitParams);
}

void oled_sendI2CData(uint8_t *const restrict data, const uint8_t size) {
	uint8_t emptyBuffer[1];

	I2C_TransferSeq_TypeDef seq;
	seq.addr = oled_I2C_address << 1; // 7-bit address.
	seq.flags = I2C_FLAG_WRITE; // Write bytes from `buf[0]`.
	seq.buf[0].data = data;
	seq.buf[0].len = size;
	seq.buf[1].data = emptyBuffer;
	seq.buf[1].len = 0;

	#ifndef JOIN_I2C_BUS
	// If not using the same bus with other I2C devices.
	I2CSPM_Transfer(I2C0, &seq);
	#else
	I2CSPM_Transfer(I2C1, &seq);
	#endif
}

void oled_initialize() {
    #ifndef JOIN_I2C_BUS
		// If not using the same bus with other I2C devices.
        oled_initalizeI2CInterface();
    #endif

    uint8_t oled_commands[] = {
          OLED_CONTROL_BYTE_CMD_STREAM,
          OLED_CMD_DISPLAY_OFF,
          OLED_CMD_SET_MUX_RATIO,
          0x3F,
          OLED_CMD_SET_DISPLAY_OFFSET,
          0x00,
          OLED_CMD_SET_DISPLAY_START_LINE,
          OLED_CMD_SET_SEGMENT_REMAP,
          OLED_CMD_SET_COM_SCAN_MODE,
          OLED_CMD_SET_COM_PIN_MAP,
          0x12,
          OLED_CMD_SET_CONTRAST,
          0x7F,
          OLED_CMD_DISPLAY_RAM,
          OLED_CMD_DISPLAY_NORMAL,
          OLED_CMD_SET_DISPLAY_CLK_DIV,
          0x80,
          OLED_CMD_SET_CHARGE_PUMP,
          0x14,
          OLED_CMD_SET_PRECHARGE,
          0x22,
          OLED_CMD_SET_VCOMH_DESELCT,
          0x30,
          OLED_CMD_SET_MEMORY_ADDR_MODE,
          0x00,
          OLED_CMD_DISPLAY_ON
      };
      oled_sendI2CData(oled_commands, 26);

      oled_clearScreen();
}

void oled_setPos(const uint8_t x, const uint8_t row) {
  uint8_t oled_commands[] = {
  		OLED_CONTROL_BYTE_CMD_STREAM,
        0xb0 | row,
        0x10 | x>>4,
        x & 0xF
  };
  oled_sendI2CData(oled_commands, 4);
}

// Screen writing is split into 16-byte chunks so as to reduce memory footprint.

void oled_clearScreen(void) {
	unsigned char setPosCmd[] = {OLED_CONTROL_BYTE_DATA_STREAM, OLED_CMD_SET_COLUMN_RANGE, 0, 127, OLED_CMD_SET_PAGE_RANGE, 0, 7};
	oled_sendI2CData(setPosCmd, sizeof(setPosCmd));
	for (int i = 0; i != oled_DisplayHeight; i++)
		oled_sendI2CData(oled_blankBuffer, 129);
}

void oled_clearRow(const int row) {
	oled_setPos(0, row);
	oled_sendI2CData(oled_blankBuffer, 129);
}

void oled_drawImage(const int startX, const int row, const unsigned char *const restrict image, int width) {
	oled_setPos(startX, row);
	if (startX + width > oled_DisplayWidth) width = oled_DisplayWidth - startX;
	unsigned char data[17] = {OLED_CONTROL_BYTE_DATA_STREAM};
	const int iBound = width / 16;
	for (int i = 0; i != iBound; i++) {
		memcpy(data+1, image + 16*i, 16);
		oled_sendI2CData(data, 17);
	}
	const int odd = width % 16;
	if (odd != 0) {
		memcpy(data+1, image + 16*iBound, odd);
		oled_sendI2CData(data, odd+1);
	}
}

void oled_drawTextSingleLine(const int startX, const int row, const char *const restrict text) {
	if (*text == 0) return;
	unsigned char data[17] = {OLED_CONTROL_BYTE_DATA_STREAM};
	const char *currentText = text;
	int dataPos = 1, currentX = startX, currentChar;
	oled_setPos(startX, row);
	while ((currentChar = *currentText) != 0) {
		if (currentChar < 0x20 || currentChar >= 0x80) continue;
		currentChar -= 0x20;
		int width = oled_fontCharWidth[currentChar];
		if (currentX + width > oled_DisplayWidth) break;
		if (currentX + width != oled_DisplayWidth) width++; // Character spacing.
		currentX += width;
		const char *bitmapPointer = oled_fontBitmap + currentChar*8;
		if (dataPos + width >= 17) {
			const int amount = 17 - dataPos;
			memcpy(data + dataPos, bitmapPointer, amount);
			oled_sendI2CData(data, 17);
			dataPos = 1;
			width -= amount;
			bitmapPointer += amount;
		}
		memcpy(data + dataPos, bitmapPointer, width);
		dataPos += width;
		currentText++;
	}
	if (dataPos != 1)
		oled_sendI2CData(data, dataPos);
}

void oled_drawTextMultiline(const int startX, const int startRow, const char *const restrict text) {
	if (*text == 0) return;
	unsigned char data[17] = {OLED_CONTROL_BYTE_DATA_STREAM};
	int
		dataPos = 1,
		currentX = startX, currentRow = startRow,
		trackX,
		currentChar;
	const char *currentText = text, *trackText;
	oled_setPos(currentX, currentRow);
	do {
		trackText = currentText;
		trackX = currentX;
		while (true) {
			currentChar = *trackText;
			if (currentChar == ' ' || currentChar == '\n' || currentChar == 0) break;
			if (currentChar < 0x20 || currentChar >= 0x80) continue;
			trackX += oled_fontCharWidth[currentChar - 0x20];
			if (trackX > oled_DisplayWidth) {
				if (dataPos != 1) {
					oled_sendI2CData(data, dataPos);
					dataPos = 1;
				}
				if (currentRow == 7) return; // Clipped out of the screen.
				currentX = 0;
				currentRow++;
				oled_setPos(currentX, currentRow);
				do {
					trackText++;
					currentChar = *trackText;
				} while (currentChar != ' ' && currentChar != 0);
				break;
			} else trackX++; // Character spacing.
			trackText++;
		}
		while (currentText != trackText) {
			currentChar = *currentText;
			if (currentChar < 0x20 || currentChar >= 0x80) continue;
			currentChar -= 0x20;
			int width = oled_fontCharWidth[currentChar];
			if (currentX + width > oled_DisplayWidth) {
				if (dataPos != 1) { // Send any data from the previous row.
					oled_sendI2CData(data, dataPos);
					dataPos = 1;
				}
				if (currentRow == 7) return; // Clipped outside the screen.
				currentRow++;
				currentX = 0;
				oled_setPos(currentX, currentRow);
			}
			if (currentX + width != oled_DisplayWidth) width++; // Character spacing.
			currentX += width;
			const char *bitmapPointer = oled_fontBitmap + currentChar*8;
			if (dataPos + width >= 17) {
				const int amount = 17 - dataPos;
				memcpy(data + dataPos, bitmapPointer, amount);
				oled_sendI2CData(data, 17);
				dataPos = 1;
				width -= amount;
				bitmapPointer += amount;
			}
			memcpy(data + dataPos, bitmapPointer, width);
			dataPos += width;
			currentText++;
		}
		if (*currentText == ' ') {
			int width = oled_fontCharWidth[0] + 1; // + 1 for character spacing.
			currentX += width;
			if (currentX < oled_DisplayWidth) {
				if (dataPos + width >= 17) {
					const int amount = 17 - dataPos;
					memset(data + dataPos, 0, amount);
					oled_sendI2CData(data, 17);
					dataPos = 1;
					width -= amount;
				}
				memset(data + dataPos, 0, width);
				dataPos += width;
			}
			currentText++;
		} else if (*currentText == '\n') {
			if (dataPos != 1) {
				oled_sendI2CData(data, dataPos);
				dataPos = 1;
			}
			if (currentRow == 7) return; // Clipped out of the screen.
			currentX = 0;
			currentRow++;
			currentText++;
			oled_setPos(currentX, currentRow);
		}
	} while (*currentText != 0);
	if (dataPos != 1)
		oled_sendI2CData(data, dataPos);
}

int oled_getTextWidth(const char *const restrict text) {
	const char *currentText = text;
	int res = 0, currentChar;
	while ((currentChar = *currentText) != 0) {
		if (currentChar < 0x20 || currentChar >= 0x80) continue;
		res += oled_fontCharWidth[currentChar - 0x20] + 1; // + 1 for character spacing.
		currentText++;
	}
	return res == 0 ? 0 : res - 1; // Remove the last character's spacing.
}
