/*
 * arducam_dev_chip.h
 *
 *  Created on: Aug 18, 2024
 *      Author: Fongberg
 */

#ifndef ARDUCAM_DEV_CHIP_H_
#define ARDUCAM_DEV_CHIP_H_

#define CHIP_REG_WRITE_BIT                  (1<<7)

/* registers */
#define ARDUCHIP_TEST1                      0x00  //TEST register
#define ARDUCHIP_FRAMES                     0x01  //Frame count register
#define ARDUCHIP_TIM                        0x03  //Timing control
#define ARDUCHIP_FIFO                       0x04  //FIFO control register
#define ARDUCHIP_TEST_MODE                  0x05  //Test mode register
#define ARDUCHIP_GPIO                       0x06  //GPIO Write Register
#define ARDUCHIP_RESET_CLPD                 0x07  //Reset CLPD Register
#define ARDUCHIP_BURST_FIFO_READ            0x3C  //Burst FIFO read operation
#define ARDUCHIP_SINGLE_FIFO_READ           0x3D  //Single FIFO read operation
#define ARDUCHIP_REV                        0x40  //ArduCHIP revision
#define ARDUCHIP_STATUS                     0x41  //Vsync status and write FIFO done flag
#define ARDUCHIP_FIFO_SIZE1                 0x42  //Camera write FIFO size[7:0]
#define ARDUCHIP_FIFO_SIZE2                 0x43  //Camera write FIFO size[15:8]
#define ARDUCHIP_FIFO_SIZE3                 0x44  //Camera write FIFO size[23:16]
#define ARDUCHIP_FIFO_FULL                  0x45  //Fifo full
#define ARDUCHIP_VERSION_YEAR               0x46  //Arduchip verion year-2000
#define ARDUCHIP_VERSION_MONTH              0x47  //Arduchip version month
#define ARDUCHIP_VERSION_DATE               0x46  //Arduchip version date

/* register fields */
#define ARDUCHIP_TIM_HSYNC_BIT              (1<<0)
#define ARDUCHIP_TIM_VSYNC_BIT              (1<<1)
#define ARDUCHIP_TIM_PCLK_BIT               (1<<3)

#define ARDUCHIP_TEST_MODE_MODE_BIT         (1<<0)

#define ARDUCHIP_FIFO_CLEAR_BIT             (1<<0)
#define ARDUCHIP_FIFO_START_BIT             (1<<1)
#define ARDUCHIP_FIFO_RESET_BIT             (1<<4)

//documentation for these bits is bad, hard to know what's going on
#define ARDUCHIP_GPIO_RESET_BIT             (1<<0)
#define ARDUCHIP_GPIO_STANDBY_BIT           (1<<1)
#define ARDUCHIP_GPIO_POWER_DISABLE_BIT     (1<<2)

#define ARDUCHIP_RESET_CLPD_RESET_BIT       (1<<7)

#define ARDUCHIP_STATUS_VSYNC_BIT           (1<<0)
#define ARDUCHIP_STATUS_EXT_TRIGGER_BIT     (1<<1)
#define ARDUCHIP_STATUS_FIFO_DONE_BIT       (1<<3)

#endif /* ARDUCAM_DEV_CHIP_H_ */
