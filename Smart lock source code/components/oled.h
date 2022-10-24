#ifndef SMARTLOCK_COMPONENTS_OLED_H
#define SMARTLOCK_COMPONENTS_OLED_H

#include <stdint.h>

#define oled_DisplayWidth 128
#define oled_DisplayHeight 64

extern unsigned char oled_blankBuffer[129];

void oled_initialize();
void oled_sendI2CData(uint8_t *const restrict data, const uint8_t size);

void oled_clearScreen(void);
void oled_clearRow(const int row);
void oled_setPos(const uint8_t x, const uint8_t row);

void oled_drawImage(const int startX, const int row, const unsigned char *const restrict image, int width);
void oled_drawTextSingleLine(const int startX, const int startRow, const char *const restrict text);
void oled_drawTextMultiline(const int startX, const int row, const char *const restrict text);

int oled_getTextWidth(const char *const restrict text);

#endif // SMARTLOCK_COMPONENTS_OLED_H
