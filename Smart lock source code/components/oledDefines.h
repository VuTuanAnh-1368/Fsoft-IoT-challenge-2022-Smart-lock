#ifndef SMARTLOCK_COMPONENTS_OLEDDEFINES_H
#define SMARTLOCK_COMPONENTS_OLEDDEFINES_H

#define OLED_CONTROL_BYTE_CMD_STREAM  0x00
#define OLED_CONTROL_BYTE_DATA_STREAM 0x40

// Fundamental commands (page 28).
#define OLED_CMD_SET_CONTRAST     0x81  // follow with 0x7F
#define OLED_CMD_DISPLAY_RAM      0xA4
#define OLED_CMD_DISPLAY_ALLON      0xA5
#define OLED_CMD_DISPLAY_NORMAL     0xA6
#define OLED_CMD_DISPLAY_INVERTED   0xA7
#define OLED_CMD_DISPLAY_OFF      0xAE
#define OLED_CMD_DISPLAY_ON       0xAF

// Addressing commands (page 30).
#define OLED_CMD_SET_MEMORY_ADDR_MODE 0x20   // follow with 0x00 = HORZ mode = Behave like a KS108 graphic LCD
#define OLED_CMD_SET_COLUMN_RANGE   0x21     // can be used only in HORZ/VERT mode - follow with 0x00 + 0x7F = COL127
#define OLED_CMD_SET_PAGE_RANGE     0x22     // can be used only in HORZ/VERT mode - follow with 0x00 + 0x07 = PAGE7

// Hardware configuration commands (page 31).
#define OLED_CMD_SET_DISPLAY_START_LINE 0x40
#define OLED_CMD_SET_SEGMENT_REMAP    0xA1
#define OLED_CMD_SET_MUX_RATIO        0xA8   // follow with 0x3F = 64 MUX
#define OLED_CMD_SET_COM_SCAN_MODE    0xC8
#define OLED_CMD_SET_DISPLAY_OFFSET   0xD3   // follow with 0x00
#define OLED_CMD_SET_COM_PIN_MAP      0xDA   // follow with 0x12

// Timing and driving commands (page 32).
#define OLED_CMD_SET_DISPLAY_CLK_DIV  0xD5   // follow with 0x80
#define OLED_CMD_SET_PRECHARGE      0xD9     // follow with 0x22
#define OLED_CMD_SET_VCOMH_DESELCT  0xDB     // follow with 0x30

// Charge pump (page 62).
#define OLED_CMD_SET_CHARGE_PUMP  0x8D       // follow with 0x14

// No-op.
#define OLED_CMD_NOP  0xE3

// Other defines.
#define oled_I2C_address 0x3C
#define OLED_I2CPORT I2C0
#define OLED_IOPORT gpioPortC
#define OLED_SCL 2
#define OLED_SDA 0

#endif // SMARTLOCK_COMPONENTS_OLEDDEFINES_H
