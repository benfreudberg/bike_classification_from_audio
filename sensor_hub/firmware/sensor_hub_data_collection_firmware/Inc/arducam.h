/*
 * arducam.h
 * Arducam Mini 2mp Plus with OV2640
 *
 *  Created on: Aug 18, 2024
 *      Author: Fongberg
 */

#ifndef ARDUCAM_H_
#define ARDUCAM_H_

#include "gpio.h"
#include "spi.h"
#include "i2c.h"
#include <stdint.h>
#include <stdbool.h>


typedef struct {
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
    SPI_HandleTypeDef *hspi;

    I2C_HandleTypeDef *hi2c;
    uint8_t i2c_addr;
} Arducam;

uint16_t ArducamSensorGetPID(const Arducam *cam);

uint32_t ArducamChipGetFifoSize(const Arducam *cam);
bool ArducamChipFifoFull(const Arducam *cam);
void ArducamChipGetVersionDate(const Arducam *cam, uint8_t *year, uint8_t *month, uint8_t *date);
void ArducamChipWriteTestReg(const Arducam *cam, uint8_t val);
uint8_t ArducamChipReadTestReg(const Arducam *cam);

void ArducamInit(const Arducam *cam);
void ArducamCapture(const Arducam *cam);
void ArducamReadImage(const Arducam *cam, uint8_t *image_data, uint32_t image_size);

#endif /* ARDUCAM_H_ */
