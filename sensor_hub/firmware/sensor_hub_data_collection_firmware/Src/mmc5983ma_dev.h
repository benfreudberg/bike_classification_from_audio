#ifndef MMC5983MA_DEV_H_
#define MMC5983MA_DEV_H_

// Registers definitions
static const uint8_t X_OUT_0_REG    = 0x0;
static const uint8_t X_OUT_1_REG    = 0X01;
static const uint8_t Y_OUT_0_REG    = 0x02;
static const uint8_t Y_OUT_1_REG    = 0x03;
static const uint8_t Z_OUT_0_REG    = 0x04;
static const uint8_t Z_OUT_1_REG    = 0x05;
static const uint8_t XYZ_OUT_2_REG  = 0x06;
static const uint8_t T_OUT_REG      = 0x07;
static const uint8_t STATUS_REG     = 0x08;
static const uint8_t INT_CTRL_0_REG = 0x09;
static const uint8_t INT_CTRL_1_REG = 0x0a;
static const uint8_t INT_CTRL_2_REG = 0x0b;
static const uint8_t INT_CTRL_3_REG = 0x0c;
static const uint8_t PROD_ID_REG    = 0x2f;
static const uint8_t DUMMY          = 0x0;

#define MAG_READ_REG(reg)           (reg | 0x80)
#define MAG_WRITE_REG(reg)          (reg | 0x00)

// Constants definitions
static const uint8_t I2C_ADDR       = 0x30;
static const uint8_t PROD_ID        = 0x30;

// Bits definitions
#define MEAS_M                      (1 << 0)
#define MEAS_T                      (1 << 1)
#define OTP_READ_DONE               (1 << 4)
#define TM_M                        (1 << 0)
#define TM_T                        (1 << 1)
#define INT_MEAS_DONE_EN            (1 << 2)
#define SET_OPERATION               (1 << 3)
#define RESET_OPERATION             (1 << 4)
#define AUTO_SR_EN                  (1 << 5)
#define OTP_READ                    (1 << 6)
#define BW0                         (1 << 0)
#define BW1                         (1 << 1)
#define X_INHIBIT                   (1 << 2)
#define YZ_INHIBIT                  (3 << 3)
#define SW_RST                      (1 << 7)
#define CM_FREQ_0                   (1 << 0)
#define CM_FREQ_1                   (1 << 1)
#define CM_FREQ_2                   (1 << 2)
#define CMM_EN                      (1 << 3)
#define PRD_SET_0                   (1 << 4)
#define PRD_SET_1                   (1 << 5)
#define PRD_SET_2                   (1 << 6)
#define EN_PRD_SET                  (1 << 7)
#define ST_ENP                      (1 << 1)
#define ST_ENM                      (1 << 2)
#define SPI_3W                      (1 << 6)
#define X2_MASK                     (3 << 6)
#define Y2_MASK                     (3 << 4)
#define Z2_MASK                     (3 << 2)
#define XYZ_0_SHIFT                 10
#define XYZ_1_SHIFT                 2

typedef enum {
  ODR100 = 0x00, //8ms, 100Hz
  ODR200 = 0x01, //4ms, 200Hz
  ODR400 = 0x02, //2ms, 400Hz
  ODR800 = 0x03  //0.5ms, 800Hz
} MagReadSpeed;

#endif /* MMC5983MA_DEV_H_ */
