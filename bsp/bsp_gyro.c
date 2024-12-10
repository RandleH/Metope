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
#include "cmn_device.h"


#define QMI8658_INVALID_VALUE                   (0xFF)

/**
 * @brief   Device Address
 * @warning Datasheet may NOT correct. Try both slave address to check!!!
 */
#define QMI8658_L_SLAVE_ADDRESS                 (0x6B<<1)  /*!< When SA0 pin was pulled up */
#define QMI8658_H_SLAVE_ADDRESS                 (0x6A<<1)  /*!< When SA0 pin was NOT connected or pulled down */
#define QMI8658_SLAVE_ADDRESS                   QMI8658_L_SLAVE_ADDRESS


/**
 * @brief   Registers Default Value
 */
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
#define QMI8658_REG_FIFO_WTM_TH                 (0x13)
#define QMI8658_REG_FIFO_CTRL                   (0x14)
#define QMI8658_REG_FIFO_COUNT                  (0x15)
#define QMI8658_REG_FIFO_STATUS                 (0x16)
#define QMI8658_REG_FIFO_DATA                   (0x17)

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

#define QMI8658_FIFO_MAP_INT1                   (0x04)    // ctrl1

/* Temperature sensor resolution */
#define TEMPERATURE_SENSOR_RESOLUTION           (1 << 8 )  // Telperature sensor resolution (ADC)

#define SENSOR_PIN_NONE                         (-1)
#define DEV_WIRE_NONE                           (0)
#define DEV_WIRE_ERR                            (-1)
#define DEV_WIRE_TIMEOUT                        (-2)

#ifdef __cplusplus
extern "C"{
#endif


typedef enum{
  kGyroFreq_25KHz,
  kGyroFreq_300KHz
}tBspGyroI2cFreq;

typedef enum {
  qmi8658_mode_acc_only = 1,   // Mode for accelerometer-only operation.
  qmi8658_mode_gyro_only,      // Mode for gyroscope-only operation.
  qmi8658_mode_dual,           // Mode for dual accelerometer and gyroscope operation.
} qmi8658_mode_t;

typedef enum {
  acc_scale_2g = 0,    // Accelerometer scale set to ±2g.
  acc_scale_4g,        // Accelerometer scale set to ±4g.
  acc_scale_8g,        // Accelerometer scale set to ±8g.
  acc_scale_16g,       // Accelerometer scale set to ±16g.
} acc_scale_t;

/* Enum representing the output data rate (ODR) settings for the accelerometer */
typedef enum {
  acc_odr_8000,      // Accelerometer ODR set to 8000 Hz.
  acc_odr_4000,      // Accelerometer ODR set to 4000 Hz.
  acc_odr_2000,      // Accelerometer ODR set to 2000 Hz.
  acc_odr_1000,      // Accelerometer ODR set to 1000 Hz.
  acc_odr_500,       // Accelerometer ODR set to 500 Hz.
  acc_odr_250,       // Accelerometer ODR set to 250 Hz.
  acc_odr_125,       // Accelerometer ODR set to 125 Hz.
  acc_odr_62_5,      // Accelerometer ODR set to 62.5 Hz.
  acc_odr_31_25,     // Accelerometer ODR set to 31.25 Hz.
  acc_odr_128 = 12,  // Accelerometer ODR set to 128 Hz.
  acc_odr_21,        // Accelerometer ODR set to 21 Hz.
  acc_odr_11,        // Accelerometer ODR set to 11 Hz.
  acc_odr_3,         // Accelerometer ODR set to 3 Hz.
} acc_odr_t;

/* Enum representing the scale settings for the gyroscope */
typedef enum {
    gyro_scale_16dps = 0,       // Gyroscope scale set to ±16 degrees per second.
    gyro_scale_32dps,            // Gyroscope scale set to ±32 degrees per second.
    gyro_scale_64dps,            // Gyroscope scale set to ±64 degrees per second.
    gyro_scale_128dps,           // Gyroscope scale set to ±128 degrees per second.
    gyro_scale_256dps,           // Gyroscope scale set to ±256 degrees per second.
    gyro_scale_512dps,           // Gyroscope scale set to ±512 degrees per second.
    gyro_scale_1024dps,          // Gyroscope scale set to ±1024 degrees per second.
    gyro_scale_2048dps,          // Gyroscope scale set to ±2048 degrees per second.
} gyro_scale_t;

typedef enum {
  gyro_odr_8000,     // Gyroscope ODR set to 8000 Hz.
  gyro_odr_4000,     // Gyroscope ODR set to 4000 Hz.
  gyro_odr_2000,     // Gyroscope ODR set to 2000 Hz.
  gyro_odr_1000,     // Gyroscope ODR set to 1000 Hz.
  gyro_odr_500,      // Gyroscope ODR set to 500 Hz.
  gyro_odr_250,      // Gyroscope ODR set to 250 Hz.
  gyro_odr_125,      // Gyroscope ODR set to 125 Hz.
  gyro_odr_62_5,     // Gyroscope ODR set to 62.5 Hz.
  gyro_odr_31_25,    // Gyroscope ODR set to 31.25 Hz.
} gyro_odr_t;



typedef union{
  struct{
    u8 CmdDone  : 1; /*!< Bit read by Host Processor as part of CTRL9 register protocol. Used to indicate ctrl9 Command was done. */
    u8 reserved : 7;
  };
  u8 reg;
} tQmi8658RegSTATUS1;

typedef union{
  struct{
    u8 SensorDisable : 1; /*!< 0: Enables internal 2 MHz oscillator; 1: Disables internal 2 MHz oscillator */
    u8 reserved      : 4;
    u8 SPI_BE        : 1;
    u8 AI            : 1; /*!< 0: Serial interface (SPI or I2C) address do not auto increment.; 1: Serial interface (SPI or I2C) address auto increment */
    u8 SIM           : 1; /*!< 0: Enables 4-wire SPI interface; 1: Enables 3-wire SPI interface */
  };
  u8 reg;
} tQmi8658RegCTRL1;

typedef union{
  struct{
    u8 aODR : 4;
    u8 aFS  : 3;
    u8 aST  : 1;
  };
  u8 reg;
} tQmi8658RegCTRL2;


typedef union{
  struct{
    u8 gODR : 4;
    u8 gFS  : 3;
    u8 gST  : 1;
  };
  u8 reg;
} tQmi8658RegCTRL3;


typedef union{
  struct{
    u8 aEN : 1;   /*!<  1: Enable Accelerometer */
    u8 gEN : 1;   /*!<  1: Enable Gyroscope */
    u8 mEN : 1;   /*!<  1: Enable Magnetometer */
    u8 sEN : 1;   /*!<  0: Disable AttitudeEngine orientation and velocity increment computation
                        1: Enable AttitudeEngine orientation and velocity increment computation */
    u8 gSN : 1;   /*!<  0: Gyroscope in Full Mode (Drive and Sense are enabled). 
                        1: Gyroscope in Snooze Mode (only Drive enabled). 
                        This bit is effective only when gEN is set to 1. */
    u8 reserved  : 1;
    u8 sys_hs    : 1;
    u8 syncSmpl  : 1;
  };
  u8 reg;
} tQmi8658RegCTRL7;


/**
 * @todo
 */
void bsp_qmi8658_set_i2c_freq( tBspGyroI2cFreq f){
  while(1); // Unimplemented
}

/**
 * @brief
 * @param [in] reg - Register address
 * @param [in] buf - Received data
 * @param [in] len - Num of items
 * @return `SUCCESS` | `ERROR`
 * @note
 *  When only accelerometer is enabled, I2C master operates at 25 kHz. When gyroscope is enabled, I2C master
operates at 300 kHz.
 * @addtogroup MachineDependent
 */
STATIC cmnBoolean_t bsp_qmi8658_i2c_polling_send( u8 reg, const u8 *buf, u8 len){
  
  if(HAL_OK!=HAL_I2C_Mem_Write( &hi2c1, QMI8658_SLAVE_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, (u8*)buf, len, HAL_MAX_DELAY)){
    return ERROR;
  }
  return SUCCESS;
}


/**
 * @brief
 * @param [in]  reg - Register address
 * @param [out] buf - Received data
 * @param [in]  len - Num of items
 * @return `SUCCESS` | `ERROR`
 * @addtogroup MachineDependent
 */
STATIC cmnBoolean_t bsp_qmi8658_i2c_polling_recv( u8 reg, u8 *buf, u8 len){
  HAL_StatusTypeDef ret = HAL_I2C_Mem_Read( &hi2c1, QMI8658_SLAVE_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, buf, len, HAL_MAX_DELAY);
  if(HAL_OK!=ret){
    return ERROR;
  }

  return SUCCESS;
}

/**
 * @brief
 * @todo
 * @param [in] reg - Register address
 * @param [in] buf - Received data
 * @param [in] len - Num of items
 * @return `SUCCESS` | `ERROR`
 * @addtogroup MachineDependent
 */
STATIC cmnBoolean_t bsp_qmi8658_i2c_dma_send( u8 reg, const u8 *buf, u8 len){
  HAL_StatusTypeDef ret = HAL_I2C_Mem_Write_DMA( &hi2c1, QMI8658_SLAVE_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, (u8*)buf, len);
  if(HAL_OK!=ret){
    return ERROR;
  }

  /**
   * @note
   *  Phase 1: DMA Request to send bytes
   *  Phase 2: DMA Interrupt will be triggered. Entering the `DMA1_Stream4_IRQHandler()`
   *  Phase 3: `HAL_DMA_IRQHandler()` will be called.
   *  Phase 4: `I2C_DMAXferCplt()` function will be called. This callback was configed during DMA requesting to send.
   *  Phase 5: `HAL_I2C_SlaveTxCpltCallback()` will be called if no error. This is a weak function.
   *  Phase 6: The escape function shall be notified to come back.
   */
  metope.dev.status.i2c1 = BUSY;
  if(metope.app.rtos.status==ON){
    xEventGroupWaitBits( metope.app.rtos.event._handle, CMN_EVENT_GYRO_TX_CPLT, pdTRUE, pdFALSE, portMAX_DELAY);
  }else{
    while(IDLE!=metope.dev.status.i2c1);
  }

  return SUCCESS;
}

/**
 * @brief
 * @todo
 * @param [in]  reg - Register address
 * @param [out] buf - Received data
 * @param [in]  len - Num of items
 * @return `SUCCESS` | `ERROR`
 * @addtogroup MachineDependent
 */
STATIC cmnBoolean_t bsp_qmi8658_i2c_dma_recv( u8 reg, u8 *buf, u8 len){
  HAL_StatusTypeDef ret = HAL_I2C_Mem_Read_DMA( &hi2c1, QMI8658_SLAVE_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, (u8*)buf, len);
  if(HAL_OK!=ret){
    return ERROR;
  }

  /**
   * @note
   *  Phase 1: DMA Request to send bytes
   *  Phase 2: DMA Interrupt will be triggered. Entering the `DMA1_Stream4_IRQHandler()`
   *  Phase 3: `HAL_DMA_IRQHandler()` will be called.
   *  Phase 4: `I2C_DMAXferCplt()` function will be called. This callback was configed during DMA requesting to send.
   *  Phase 5: `HAL_I2C_SlaveRxCpltCallback()` will be called if no error. This is a weak function.
   *  Phase 6: The escape function shall be notified to come back.
   */
  metope.dev.status.i2c1 = BUSY;
  if(metope.app.rtos.status==ON){
    xEventGroupWaitBits( metope.app.rtos.event._handle, CMN_EVENT_GYRO_TX_CPLT, pdTRUE, pdFALSE, portMAX_DELAY);
  }else{
    while(IDLE!=metope.dev.status.i2c1);
  }

  return SUCCESS;
}


/**
 * @brief Read 1 byte from register
 */
STATIC u8 bsp_qmi8658_i2c_read_reg( u8 reg){
  u8 data;
  bsp_qmi8658_i2c_polling_recv( reg, &data, 1);
  return data;
}

/**
 * @brief Read 1 byte from register
 */
STATIC void bsp_qmi8658_i2c_write_reg( u8 reg, u8 val){
  bsp_qmi8658_i2c_polling_send( reg, &val, 1);
}


/**
 * @brief Turn ON/OFF the device. (Power Switch)
 * @param [in] on_off - Switch
 * @addtogroup MachineDependent
 */
void bsp_qmi8658_switch( cmnBoolean_t on_off){
  HAL_GPIO_WritePin( GYRO_EN_GPIO_Port, GYRO_EN_Pin, on_off!=OFF);
}

void bsp_qmi8658_reset( void){
  u8 data = QMI8658_REG_RESET_DEFAULT;
  bsp_qmi8658_i2c_polling_send( QMI8658_REG_RESET, &data, sizeof(data));

  /* Wait until an interrupt occurred */
  while(metope.dev.status.B5==0);
  volatile tQmi8658RegSTATUS1 reg_status1 = {0};
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_STATUS1, &reg_status1.reg, 1);
  metope.dev.status.B5 = 0;
}

/**
 * @todo
 */
void bsp_qmi8658_select_mode(qmi8658_mode_t qmi8658_mode ) {
  u8 data;
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_CTRL7, &data, 1);
  data = (0xFC & data) | qmi8658_mode;
  bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL7, &data, 1);
}

/**
 * @todo
 */
static void bsp_qmi8658_acc_setting( acc_odr_t odr, acc_scale_t scale){
  tQmi8658RegCTRL2 reg_ctrl2;
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_CTRL2, &reg_ctrl2.reg, 1);
  reg_ctrl2.aODR = odr;
  reg_ctrl2.aFS  = scale;
  bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL2, &reg_ctrl2.reg, 1);
}


static enum tBspGyroAccSensitivity bsp_qmi8658_get_acc_scale(void){
  tQmi8658RegCTRL2 reg_ctrl2 = {.reg=bsp_qmi8658_i2c_read_reg(QMI8658_REG_CTRL2)};

  // 0 -> 1<<14
  // 1 -> 1<<13
  // 2 -> 1<<12
  // 3 -> 1<<11
  if(reg_ctrl2.aFS < 4){
    return (enum tBspGyroDegreeSensitivity)(1<<(14-reg_ctrl2.aFS));
  }else{
    return ACC_SCALE_SENSITIVITY_INVALID;
  }
}

/**
 * @todo
 */
static void bsp_qmi8658_gyro_setting(gyro_odr_t odr, gyro_scale_t scale){
  tQmi8658RegCTRL3 reg_ctrl3;
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_CTRL3, &reg_ctrl3.reg, 1);
  reg_ctrl3.gODR = odr;
  reg_ctrl3.gFS  = scale;
  bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL3, &reg_ctrl3.reg, 1);
}


static enum tBspGyroDegreeSensitivity bsp_qmi8658_get_gyro_scale(void){
  tQmi8658RegCTRL3 reg_ctrl3 = {.reg=bsp_qmi8658_i2c_read_reg(QMI8658_REG_CTRL3)};

  // 0 -> 1<<11
  // 1 -> 1<<10
  // 2 -> 1<< 9
  // ...
  // 7 -> 1<< 4
  return (enum tBspGyroDegreeSensitivity)(1<<(11-reg_ctrl3.gFS));
}


/**
 * @brief QMI8658 Device Initialization
 */
cmnBoolean_t bsp_qmi8658_init(void){
  bsp_qmi8658_switch(ON);
  bsp_qmi8658_reset();
  bsp_qmi8658_select_mode(qmi8658_mode_dual);
  bsp_qmi8658_acc_setting(acc_odr_8000, acc_scale_2g);
  bsp_qmi8658_gyro_setting(gyro_odr_8000, gyro_scale_16dps);

  /**
   * @note: Validate the configuration
   */
  u8 data = QMI8658_INVALID_VALUE;
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_CTRL7, &data, 1);
  if((data&0x03) == qmi8658_mode_dual){
    return SUCCESS;
  }else{
    return ERROR;
  }
}


u8 bsp_qmi8658_get_chip_id(void){
  u8 data = 0x00;
  if(ERROR==bsp_qmi8658_i2c_polling_recv(QMI8658_REG_REVISION, &data, 1)){
    data = 0xFF;
  }
  return data;
}

u8 bsp_qmi8658_get_who_am_i(void){
  u8 data = 0x00;
  if(ERROR==bsp_qmi8658_i2c_polling_recv(QMI8658_REG_WHOAMI, &data, 1)){
    data = 0xFF;
  }
  return data;
}

/**
 * @brief Update the date for once
 */
cmnBoolean_t bsp_qmi8658_update( tBspGyroData *data){
  // read accelerometer data
  data->acc.x = (((int16_t)bsp_qmi8658_i2c_read_reg(QMI8658_REG_AX_H) << 8) | bsp_qmi8658_i2c_read_reg(QMI8658_REG_AX_L));
  data->acc.y = (((int16_t)bsp_qmi8658_i2c_read_reg(QMI8658_REG_AY_H) << 8) | bsp_qmi8658_i2c_read_reg(QMI8658_REG_AY_L));
  data->acc.z = (((int16_t)bsp_qmi8658_i2c_read_reg(QMI8658_REG_AZ_H) << 8) | bsp_qmi8658_i2c_read_reg(QMI8658_REG_AZ_L));
  data->acc.acc_sensitivity = bsp_qmi8658_get_acc_scale();

  // read gyroscope data
  data->gyro.x = (((int16_t)bsp_qmi8658_i2c_read_reg(QMI8658_REG_GX_H) << 8) | bsp_qmi8658_i2c_read_reg(QMI8658_REG_GX_L));
  data->gyro.y = (((int16_t)bsp_qmi8658_i2c_read_reg(QMI8658_REG_GY_H) << 8) | bsp_qmi8658_i2c_read_reg(QMI8658_REG_GY_L));
  data->gyro.z = (((int16_t)bsp_qmi8658_i2c_read_reg(QMI8658_REG_GZ_H) << 8) | bsp_qmi8658_i2c_read_reg(QMI8658_REG_GZ_L));
  data->gyro.deg_sensitivity = bsp_qmi8658_get_gyro_scale();

  // read temperature data
  data->temperature = (((int16_t)bsp_qmi8658_i2c_read_reg(QMI8658_REG_TEMPERATURE_H) << 8) | bsp_qmi8658_i2c_read_reg(QMI8658_REG_TEMPERATURE_L));

  return true;
}


/**
 * @brief Keep updating the data
 */
cmnBoolean_t bsp_qmi8658_updating( tBspGyroData *data){
  return false;
}

/**
 * @brief Stop updating the data
 */
cmnBoolean_t bsp_qmi8658_stop_updating(void){
  return false;
}

u8 bsp_qmi8658_debug_read_reg(u8 reg){
  return bsp_qmi8658_i2c_read_reg(reg);
}

void bsp_qmi8658_debug_write_reg(u8 reg, u8 val){
  bsp_qmi8658_i2c_write_reg(reg, val);
}

#ifdef __cplusplus
}
#endif