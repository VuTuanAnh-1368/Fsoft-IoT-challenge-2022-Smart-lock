#include <stdint.h>

#include <sl_i2cspm.h>

// I2C address.
#define MLX90614_I2CADDR 0x5A

// RAM.
#define MLX90614_RAWIR1 0x04
#define MLX90614_RAWIR2 0x05
#define MLX90614_TA 0x06
#define MLX90614_TOBJ1 0x07
#define MLX90614_TOBJ2 0x08

// EEPROM.
#define MLX90614_TOMAX 0x20
#define MLX90614_TOMIN 0x21
#define MLX90614_PWMCTRL 0x22
#define MLX90614_TARANGE 0x23
#define MLX90614_EMISS 0x24
#define MLX90614_CONFIG 0x25
#define MLX90614_ADDR 0x0E
#define MLX90614_ID1 0x3C
#define MLX90614_ID2 0x3D
#define MLX90614_ID3 0x3E
#define MLX90614_ID4 0x3F

uint16_t read16(uint8_t address) {
	uint16_t ret;
	uint8_t command[10];
	uint8_t returnData[4];
	command[0] = address; // Send register address to read.
	I2C_TransferSeq_TypeDef seq; 
	seq.addr = MLX90614_I2CADDR << 1; // 7-bit address.
	seq.buf[0].data = command;
	seq.buf[0].len = 1;
	seq.buf[1].data = returnData;
	seq.buf[1].len = 3;
	seq.flags = I2C_FLAG_WRITE_READ; // Data written from `buf[0].data` and read into `buf[1].data`.
	I2CSPM_Transfer(I2C1, &seq);       
	// Receive data.
	ret = seq.buf[1].data[0];
	ret |= seq.buf[1].data[1] << 8;
	return ret;
}

float readTemp(uint8_t reg) {
	// The register contains temperature in units of 0,02 K.
	return read16(reg) / 50.f - 273.15f;
}

float readObjectTempC(void) {
	return readTemp(MLX90614_TOBJ1);
}

float readAmbientTempC(void) {
	return readTemp(MLX90614_TA);
}
