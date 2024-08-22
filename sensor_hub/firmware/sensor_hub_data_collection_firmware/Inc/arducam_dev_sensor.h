/*
 * arducam_dev_sensor.h
 *
 *  Created on: Aug 18, 2024
 *      Author: Fongberg
 */

#ifndef ARDUCAM_DEV_SENSOR_H_
#define ARDUCAM_DEV_SENSOR_H_

#include <stdint.h>

//register addresses
#define OV2640_SENSOR_ID_HIGH_ADDR  0x0A
#define OV2640_SENSOR_ID_LOW_ADDR   0x0B
//regisster values
#define OV2640_SENSOR_ID_HIGH_VAL   0x26
#define OV2640_SENSOR_ID_LOW_VAL    0x42

typedef struct {
    uint8_t reg;
    uint8_t val;
} SensorRegValPair;

extern const SensorRegValPair OV2640_JPEG_INIT[];
extern const SensorRegValPair OV2640_YUV422[];
extern const SensorRegValPair OV2640_JPEG[];
extern const SensorRegValPair OV2640_320x240_JPEG[];
extern const SensorRegValPair OV2640_1024x768_JPEG[];
extern const SensorRegValPair OV2640_1600x1200_JPEG[];

#endif /* ARDUCAM_DEV_SENSOR_H_ */
