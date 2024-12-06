/**
 ******************************************************************************
 * @file    bsp_gyro.c
 * @author  RandleH
 * @brief   Board Support Package Delivery - QMI8658C
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 RandleH.
 * All rights reserved.
 *
 * This software component is licensed by RandleH under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
*/


#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  #include "stm32f4xx_hal.h"
  #include "stm32f4xx_hal_i2c.h"
#endif
#include "global.h"
#include "bsp_gyro.h"



// @brief  device address
#define QMI8658_L_SLAVE_ADDRESS                 (0x6B)
#define QMI8658_H_SLAVE_ADDRESS                 (0x6A)


// @brief  registers default value
#define QMI8658_REG_WHOAMI_DEFAULT              (0x05)
#define QMI8658_REG_STATUS_DEFAULT              (0x03)
#define QMI8658_REG_RESET_DEFAULT               (0xB0)


//* General Purpose Registers
#define QMI8658_REG_WHOAMI                      (0x00)
#define QMI8658_REG_REVISION                    (0x01)


//* Setup and Control Registers
#define QMI8658_REG_CTRL1                       (0x02)
#define QMI8658_REG_CTRL2                       (0x03)
#define QMI8658_REG_CTRL3                       (0x04)
// Reserved                                     (0x05)
#define QMI8658_REG_CTRL5                       (0x06)
// Reserved                                     (0x07)
#define QMI8658_REG_CTRL7                       (0x08)
#define QMI8658_REG_CTRL8                       (0x09)
#define QMI8658_REG_CTRL9                       (0x0A)

//* Host Controlled Calibration Registers (See CTRL9, Usage is Optional)
#define QMI8658_REG_CAL1_L                      (0x0B)
#define QMI8658_REG_CAL1_H                      (0x0C)
#define QMI8658_REG_CAL2_L                      (0x0D)
#define QMI8658_REG_CAL2_H                      (0x0E)
#define QMI8658_REG_CAL3_L                      (0x0F)
#define QMI8658_REG_CAL3_H                      (0x10)
#define QMI8658_REG_CAL4_L                      (0x11)
#define QMI8658_REG_CAL4_H                      (0x12)

//* FIFO Registers
#define QMI8658_REG_FIFO_WTM_TH                  (0x13)
#define QMI8658_REG_FIFO_CTRL                    (0x14)
#define QMI8658_REG_FIFO_COUNT                   (0x15)
#define QMI8658_REG_FIFO_STATUS                  (0x16)
#define QMI8658_REG_FIFO_DATA                    (0x17)

//* Status Registers
#define QMI8658_REG_STATUS_INT                  (0x2D)
#define QMI8658_REG_STATUS0                     (0x2E)
#define QMI8658_REG_STATUS1                     (0x2F)

//* Timestamp Register
#define QMI8658_REG_TIMESTAMP_L                 (0x30)
#define QMI8658_REG_TIMESTAMP_M                 (0x31)
#define QMI8658_REG_TIMESTAMP_H                 (0x32)

//* Data Output Registers (16 bits 2’s Complement Except COD Sensor Data)
#define QMI8658_REG_TEMPERATURE_L               (0x33)
#define QMI8658_REG_TEMPERATURE_H               (0x34)
#define QMI8658_REG_AX_L                        (0x35)
#define QMI8658_REG_AX_H                        (0x36)
#define QMI8658_REG_AY_L                        (0x37)
#define QMI8658_REG_AY_H                        (0x38)
#define QMI8658_REG_AZ_L                        (0x39)
#define QMI8658_REG_AZ_H                        (0x3A)
#define QMI8658_REG_GX_L                        (0x3B)
#define QMI8658_REG_GX_H                        (0x3C)
#define QMI8658_REG_GY_L                        (0x3D)
#define QMI8658_REG_GY_H                        (0x3E)
#define QMI8658_REG_GZ_L                        (0x3F)
#define QMI8658_REG_GZ_H                        (0x40)

//* COD Indication and General Purpose Registers

// Calibration-On-Demand status register
#define QMI8658_REG_COD_STATUS                  (0x46)
#define QMI8658_REG_DQW_L                       (0x49)
#define QMI8658_REG_DQW_H                       (0x4A)
#define QMI8658_REG_DQX_L                       (0x4B)
#define QMI8658_REG_DQX_H                       (0x4C)

#define QMI8658_REG_DQY_L                       (0x4D)
#define QMI8658_REG_DQY_H                       (0x4E)
#define QMI8658_REG_DQZ_L                       (0x4F)
#define QMI8658_REG_DQZ_H                       (0x50)

#define QMI8658_REG_DVX_L                       (0x51)
#define QMI8658_REG_DVX_H                       (0x52)
#define QMI8658_REG_DVY_L                       (0x53)
#define QMI8658_REG_DVY_H                       (0x54)
#define QMI8658_REG_DVZ_L                       (0x55)
#define QMI8658_REG_DVZ_H                       (0x56)

//* Activity Detection Output Registers
#define QMI8658_REG_TAP_STATUS                  (0x59)
#define QMI8658_REG_STEP_CNT_LOW                (0x5A)
#define QMI8658_REG_STEP_CNT_MID                (0x5B)
#define QMI8658_REG_STEP_CNT_HIGH               (0x5C)
#define QMI8658_REG_RESET                       (0x60)

//* Reset Register
#define QMI8658_REG_RST_RESULT                  (0x4D)
#define QMI8658_REG_RST_RESULT_VAL              (0x80)

#define STATUS0_ACCEL_AVAIL                     (0x01)
#define STATUS0_GYRO_AVAIL                      (0x02)
#define QMI8658_ACCEL_LPF_MASK                  (0xF9)
#define QMI8658_GYRO_LPF_MASK                   (0x9F)


#define QMI8658_ACCEL_EN_MASK                   (0x01)
#define QMI8658_GYRO_EN_MASK                    (0x02)
#define QMI8658_ACCEL_GYRO_EN_MASK              (0x03)


#define QMI8658_FIFO_MAP_INT1           0x04    // ctrl1



/**
 * @brief
 * @addtogroup MachineDependent
 */
STATIC cmnBoolean_t bsp_gyro_i2c_polling_send( const uint8_t *buf, size_t nItems, size_t nTimes){
  if(HAL_OK!=HAL_I2C_Master_Transmit( &hi2c1, QMI8658_L_SLAVE_ADDRESS, (u8*)buf, nItems,HAL_MAX_DELAY)){
    return ERROR;
  }
  return SUCCESS;
}


/**
 * @brief
 * @param [out] buf    - Received data
 * @param [in]  nItems - Num of items
 * @param [in]  reg    - Register address
 * @addtogroup MachineDependent
 */
STATIC cmnBoolean_t bsp_gyro_i2c_polling_recv( uint8_t *buf, size_t nItems, uint8_t reg){
  if(HAL_OK!=HAL_I2C_Master_Receive( &hi2c1, QMI8658_L_SLAVE_ADDRESS, buf, nItems, HAL_MAX_DELAY)){
    return ERROR;
  }
  return SUCCESS;
}

/**
 * @brief
 * @addtogroup MachineDependent
 */
STATIC cmnBoolean_t bsp_gyro_i2c_dma_send( const uint8_t *buf, size_t nItems, size_t nTimes){
  return ERROR;
}



void bsp_gyro_reset( void){
  u8 data[] = {
    QMI8658_REG_RESET, \
    QMI8658_REG_RESET_DEFAULT
  };
  bsp_gyro_i2c_polling_send(data, sizeof(data), 1);
}


u8 bsp_gyro_get_chip_id(void){
  u8 data = 0xFF;
  bsp_gyro_i2c_polling_recv( &data, 1, QMI8658_REG_REVISION);
  return data;
}