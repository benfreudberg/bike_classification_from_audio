/*
 * arducam.c
 *
 *  Created on: Aug 18, 2024
 *      Author: Fongberg
 */

#include "arducam.h"
#include "arducam_dev_sensor.h"
#include "arducam_dev_chip.h"

/* private sensor functions */

static void SensorWriteReg(const Arducam *cam, uint8_t reg_id, uint8_t val) {
  uint8_t tx_data[2] = {reg_id, val};

  HAL_I2C_Master_Transmit(
      cam->hi2c,
      cam->i2c_addr << 1,
      tx_data,
      2,
      1);
}

static uint8_t SensorReadReg(const Arducam *cam, uint8_t reg_id) {
  uint8_t tx_data = reg_id;
  uint8_t val;

  HAL_I2C_Master_Transmit(
      cam->hi2c,
      cam->i2c_addr << 1,
      &tx_data,
      1,
      1);

  HAL_I2C_Master_Receive(
      cam->hi2c,
      cam->i2c_addr << 1,
      &val,
      1,
      1);
  return val;
}

static void SensorSetRegBank0(const Arducam *cam) {
  SensorWriteReg(cam, 0xff, 0x00);
}

static void SensorSetRegBank1(const Arducam *cam) {
  SensorWriteReg(cam, 0xff, 0x01);
}

static void SensorWriteRegList(const Arducam *cam, const SensorRegValPair * const reg_val_list) {
  const SensorRegValPair * reg_val_pair = reg_val_list;
  while ((reg_val_pair->reg != 0xff) || (reg_val_pair->val != 0xff)) {
    SensorWriteReg(cam, reg_val_pair->reg, reg_val_pair->val);
    reg_val_pair++;
  }
}

/* private chip functions */

static void ChipWriteReg(const Arducam *cam, uint8_t reg_id, uint8_t val) {
  uint8_t tx_data[2] = {reg_id|CHIP_REG_WRITE_BIT, val};
  uint8_t rx_data[2];

  HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(cam->hspi,
                          tx_data,
                          rx_data,
                          2,
                          1);
  HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_SET);
}

static uint8_t ChipReadReg(const Arducam *cam, uint8_t reg_id) {
  uint8_t tx_data[2] = {reg_id, 0};
  uint8_t rx_data[2];

  HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(cam->hspi,
                          tx_data,
                          rx_data,
                          2,
                          1);
  HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_SET);
  return rx_data[1];
}

static void ChipBurstReadFifo(const Arducam *cam, uint8_t *data, uint32_t length) {
  uint8_t command = ARDUCHIP_BURST_FIFO_READ;
  uint8_t tx_data[length];

  HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(cam->hspi,
                          &command,
                          data,
                          1,
                          1);
  HAL_SPI_TransmitReceive(cam->hspi,
                          tx_data,
                          data,
                          length,
                          1);
  HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_SET);
}

/* public functions */

uint16_t ArducamSensorGetPID(const Arducam *cam) {
  uint8_t sensor_id_high, sensor_id_low;
  SensorSetRegBank1(cam);
  sensor_id_high = SensorReadReg(cam, OV2640_SENSOR_ID_HIGH);
  sensor_id_low = SensorReadReg(cam, OV2640_SENSOR_ID_LOW);
  return sensor_id_high << 8 | sensor_id_low;
}


uint32_t ArducamChipGetFifoSize(const Arducam *cam) {
  uint8_t read_vals[3];
  read_vals[0] = ChipReadReg(cam, ARDUCHIP_FIFO_SIZE1);
  read_vals[1] = ChipReadReg(cam, ARDUCHIP_FIFO_SIZE2);
  read_vals[2] = ChipReadReg(cam, ARDUCHIP_FIFO_SIZE3);
  return read_vals[0] | read_vals[1] << 8 | read_vals[2] << 16;
}

bool ArducamChipFifoFull(const Arducam *cam) {
  return (bool)ChipReadReg(cam, ARDUCHIP_FIFO_FULL);
}

void ArducamChipGetVersionDate(const Arducam *cam, uint8_t *year, uint8_t *month, uint8_t *date) {
  *year  = ChipReadReg(cam, ARDUCHIP_VERSION_YEAR);
  *month = ChipReadReg(cam, ARDUCHIP_VERSION_MONTH);
  *date  = ChipReadReg(cam, ARDUCHIP_VERSION_DATE);
}

void ArducamChipWriteTestReg(const Arducam *cam, uint8_t val) {
  ChipWriteReg(cam, ARDUCHIP_TEST1, val);
}

uint8_t ArducamChipReadTestReg(const Arducam *cam) {
  return ChipReadReg(cam, ARDUCHIP_TEST1);
}


void ArducamInit(const Arducam *cam) {
  //reset CLPD?
  ChipWriteReg(cam, ARDUCHIP_RESET_CLPD, ARDUCHIP_RESET_CLPD_RESET_BIT);
  HAL_Delay(100);
  ChipWriteReg(cam, ARDUCHIP_RESET_CLPD, 0);
  HAL_Delay(100);

  SensorSetRegBank1(cam);
  SensorWriteReg(cam, 0x12, 0x80);
  HAL_Delay(100);
  SensorWriteRegList(cam, OV2640_JPEG_INIT);
  SensorWriteRegList(cam, OV2640_YUV422);
  SensorWriteRegList(cam, OV2640_JPEG);
  SensorSetRegBank1(cam);
  SensorWriteReg(cam, 0x15, 0x00);
  SensorWriteRegList(cam, OV2640_320x240_JPEG);

  HAL_Delay(1000);
  ChipWriteReg(cam, ARDUCHIP_FIFO, ARDUCHIP_FIFO_CLEAR_BIT);
  ChipWriteReg(cam, ARDUCHIP_FRAMES, 0);
}

void ArducamCapture(const Arducam *cam) {
  uint8_t status = ChipReadReg(cam, ARDUCHIP_STATUS);


  ChipWriteReg(cam, ARDUCHIP_FIFO, ARDUCHIP_FIFO_CLEAR_BIT);
  ChipWriteReg(cam, ARDUCHIP_FIFO, ARDUCHIP_FIFO_START_BIT);

  while (!(status & ARDUCHIP_STATUS_FIFO_DONE_BIT)) {
    status = ChipReadReg(cam, ARDUCHIP_STATUS);
  }
}

void ArducamReadImage(const Arducam *cam, uint8_t *image_data, uint32_t image_size) {
  ChipBurstReadFifo(cam, image_data, image_size);
}
