/**
 ******************************************************************************
 * @file    bsp_gyro.c
 * @author  RandleH
 * @brief   Board Support Package Delivery - QMI8658A/QMI8658C
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 RandleH.
 * All rights reserved.
 *
 * This software component is licensed by RandleH under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
*/

/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  #include "stm32f4xx_hal.h"
  #include "stm32f4xx_hal_i2c.h"
  #include "stm32f4xx_hal_dma.h"
#endif
#include "global.h"
#include "bsp_gyro.h"
#include "cmn_device.h"
#include "cmn_delay.h"

/* ************************************************************************** */
/*                                  Macros                                    */
/* ************************************************************************** */
#define QMI8658_INVALID_VALUE                   (0xFF)

/**
 * @brief Device Address
 * @attention 
 *  Datasheet may NOT correct. Try both slave address to check!!!
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
#if (defined QMI8658C)
  #define QMI8658_REG_CTRL4                       (0x05)
#endif
#define QMI8658_REG_CTRL5                       (0x06)
#if (defined QMI8658C)
  #define QMI8658_REG_CTRL6                       (0x07)
#endif
#define QMI8658_REG_CTRL7                       (0x08)
#define QMI8658_REG_CTRL8                       (0x09)
#define QMI8658_REG_CTRL9                       (0x0A)

#define QMI8658_CTRL9_CMD_ACK                     (0x00) // [Ctrl9 ] Acknowledgement. Host acknowledges to QMI8658, to end the protocol.
#define QMI8658_CTRL9_CMD_RST_FIFO                (0x04) // [Ctrl9 ] Reset FIFO from Host
#define QMI8658_CTRL9_CMD_REQ_FIFO                (0x05) // [Ctrl9R] Get FIFO data from Device
#define QMI8658_CTRL9_CMD_WRITE_WOM_SETTING       (0x08) // [WCtrl9] Set up and enable Wake on Motion (WoM)
#define QMI8658_CTRL9_CMD_ACCEL_HOST_DELTA_OFFSET (0x09) // [WCtrl9] Change accelerometer offset
#define QMI8658_CTRL9_CMD_GYRO_HOST_DELTA_OFFSET  (0x0A) // [WCtrl9] Change gyroscope offset
#define QMI8658_CTRL9_CMD_CONFIGURE_TAP           (0x0C) // [WCtrl9] Configure Tap detection
#define QMI8658_CTRL9_CMD_CONFIGURE_PEDOMETER     (0x0D) // [WCtrl9] Configure Pedometer
#define QMI8658_CTRL9_CMD_CONFIGURE_MOTION        (0x0E) // [WCtrl9] Configure Any Motion / No Motion / Significant Motion detection
#define QMI8658_CTRL9_CMD_RESET_PEDOMETER         (0x0F) // [WCtrl9] Reset pedometer count (step count)
#define QMI8658_CTRL9_CMD_COPY_USID               (0x10) // [Ctrl9R] Copy USID and FW Version to UI registers
#define QMI8658_CTRL9_CMD_SET_RPU                 (0x11) // [WCtrl9] Configures IO pull-ups
#define QMI8658_CTRL9_CMD_AHB_CLOCK_GATING        (0x12) // [WCtrl9] Internal AHB clock gating switch
#define QMI8658_CTRL9_CMD_ON_DEMAND_CALIBRATION   (0xA2) // [WCtrl9] On-Demand Calibration on gyroscope
#define QMI8658_CTRL9_CMD_APPLY_GYRO_GAINS        (0xAA) // [WCtrl9] Restore the saved Gyroscope gains


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

/**
 * @attention
 *  Based on my observation, the address for Acceleration and Angular Rate are inverted.
 *  Try both to verify which one is correct. Do NOT fully trust the datasheet.
 */
#if 0
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
#else
  #define QMI8658_REG_GX_L                        (0x35)
  #define QMI8658_REG_GX_H                        (0x36)
  #define QMI8658_REG_GY_L                        (0x37)
  #define QMI8658_REG_GY_H                        (0x38)
  #define QMI8658_REG_GZ_L                        (0x39)
  #define QMI8658_REG_GZ_H                        (0x3A)
  #define QMI8658_REG_AX_L                        (0x3B)
  #define QMI8658_REG_AX_H                        (0x3C)
  #define QMI8658_REG_AY_L                        (0x3D)
  #define QMI8658_REG_AY_H                        (0x3E)
  #define QMI8658_REG_AZ_L                        (0x3F)
  #define QMI8658_REG_AZ_H                        (0x40)
#endif

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

/* Temperature sensor resolution */
#define TEMPERATURE_SENSOR_RESOLUTION           (1<<8)  // Telperature sensor resolution (ADC)


#ifdef __cplusplus
extern "C"{
#endif


typedef enum {
  qmi8658_mode_acc_only = 1,   // Mode for accelerometer-only operation.
  qmi8658_mode_gyro_only,      // Mode for gyroscope-only operation.
  qmi8658_mode_dual,           // Mode for dual accelerometer and gyroscope operation.
} qmi8658_mode_t;

typedef enum {
  kAccScale_2g = 0,    // Accelerometer scale set to ±2g.
  kAccScale_4g,        // Accelerometer scale set to ±4g.
  kAccScale_8g,        // Accelerometer scale set to ±8g.
  kAccScale_16g,       // Accelerometer scale set to ±16g.
} tQmi8658AccScale;

/* Enum representing the output data rate (ODR) settings for the accelerometer */
typedef enum {
  kAccOdr_8000Hz,      // Accelerometer ODR set to 8000 Hz.
  kAccOdr_4000Hz,      // Accelerometer ODR set to 4000 Hz.
  kAccOdr_2000Hz,      // Accelerometer ODR set to 2000 Hz.
  kAccOdr_1000Hz,      // Accelerometer ODR set to 1000 Hz.
  kAccOdr_500Hz,       // Accelerometer ODR set to 500 Hz.
  kAccOdr_250Hz,       // Accelerometer ODR set to 250 Hz.
  kAccOdr_125Hz,       // Accelerometer ODR set to 125 Hz.
  kAccOdr_62_5Hz,      // Accelerometer ODR set to 62.5 Hz.
  kAccOdr_31_25Hz,     // Accelerometer ODR set to 31.25 Hz.
  kAccOdr_128Hz = 12,  // Accelerometer ODR set to 128 Hz.
  kAccOdr_21Hz,        // Accelerometer ODR set to 21 Hz.
  kAccOdr_11Hz,        // Accelerometer ODR set to 11 Hz.
  kAccOdr_3Hz,         // Accelerometer ODR set to 3 Hz.
} tQmi8658AccOdr;

/* Enum representing the scale settings for the gyroscope */
/**
 * @hack
 *  Find `gyro_odr_\d+`, replace with `kGyroScale_$0dps`
 */
typedef enum {
  kGyroScale_16dps = 0, // Gyroscope scale set to ±16 degrees per second.
  kGyroScale_32dps,     // Gyroscope scale set to ±32 degrees per second.
  kGyroScale_64dps,     // Gyroscope scale set to ±64 degrees per second.
  kGyroScale_128dps,    // Gyroscope scale set to ±128 degrees per second.
  kGyroScale_256dps,    // Gyroscope scale set to ±256 degrees per second.
  kGyroScale_512dps,    // Gyroscope scale set to ±512 degrees per second.
  kGyroScale_1024dps,   // Gyroscope scale set to ±1024 degrees per second.
  kGyroScale_2048dps,   // Gyroscope scale set to ±2048 degrees per second.
} tQmi8658GyroScale;

typedef enum {
  kGyroOdr_8000Hz,     // Gyroscope ODR set to 8000 Hz.
  kGyroOdr_4000Hz,     // Gyroscope ODR set to 4000 Hz.
  kGyroOdr_2000Hz,     // Gyroscope ODR set to 2000 Hz.
  kGyroOdr_1000Hz,     // Gyroscope ODR set to 1000 Hz.
  kGyroOdr_500Hz,      // Gyroscope ODR set to 500 Hz.
  kGyroOdr_250Hz,      // Gyroscope ODR set to 250 Hz.
  kGyroOdr_125Hz,      // Gyroscope ODR set to 125 Hz.
  kGyroOdr_62_5Hz,     // Gyroscope ODR set to 62.5 Hz.
  kGyroOdr_31_25Hz,    // Gyroscope ODR set to 31.25 Hz.
} tQmi8658GyroOdr;



/* ************************************************************************** */
/*                          Macros Default Settings                           */
/* ************************************************************************** */
#define QMI8658_DEFAULT_ACC_SCALE     kAccScale_2g
#define QMI8658_DEFAULT_ACC_ODR       kAccOdr_2000Hz
#define QMI8658_DEFAULT_GYRO_SCALE    kGyroScale_2048dps
#define QMI8658_DEFAULT_GYRO_ODR      kGyroOdr_2000Hz




typedef union{
#if (defined QMI8658A)
  struct{
    u8 Avail    : 1;
    u8 Locked   : 1;
    u8 Reserved : 5;
    u8 CmdDone  : 1;
  };
#elif (defined QMI8658C)
  struct{
    u8 Avail    : 1;
    u8 Locked   : 1;
    u8 Reserved : 6;
  };
#endif
  u8 reg;
} tQmi8658RegSTATUSINT;

typedef union{
#if (defined QMI8658A)
  struct{
    u8 aDA      : 1;
    u8 gDA      : 1;
    u8 Reserved : 6;
  };
#elif (defined QMI8658C)
  struct{
    u8 CmdDone  : 1; /*!< Bit read by Host Processor as part of CTRL9 register protocol. Used to indicate ctrl9 Command was done. */
    u8 Reserved : 7;
  };
#endif
  u8 reg;
} tQmi8658RegSTATUS0;

typedef union{
#if (defined QMI8658A)
  struct{
    u8 Reserved0         : 1;
    u8 TAP               : 1;
    u8 WoM               : 1;
    u8 Reserved1         : 1;
    u8 Pedometer         : 1;
    u8 AnyMotion         : 1;
    u8 NoMotion          : 1;
    u8 SignificantMotion : 1;
  };
#elif (defined QMI8658C)
  struct{
    u8 CmdDone  : 1; /*!< Bit read by Host Processor as part of CTRL9 register protocol. Used to indicate ctrl9 Command was done. */
    u8 reserved : 7;
  };
#endif
  u8 reg;
} tQmi8658RegSTATUS1;

typedef union{
#if (defined QMI8658A)
  struct{
    u8 SensorDisable : 1; /*!< 0: Enables internal 2 MHz oscillator; 1: Disables internal 2 MHz oscillator */
    u8 reserved      : 1;
    u8 FIFO_INT_SEL  : 1; /*!< 0: FIFO interrupt is mapped to INT2 pin; 1: FIFO interrupt is mapped to INT1 pin */
    u8 INT1_EN       : 1; /*!< 0: INT1 pin is high-Z mode; 1: INT1 pin output is enabled */
    u8 INT2_EN       : 1; /*!< 0: INT2 pin is high-Z mode; 1: INT2 pin output is enabled */
    u8 BE            : 1; /*!< 0: Serial interface (SPI, I2C, I3C) read data Little-Endian; 1: Serial interface (SPI, I2C, I3C) read data Big-Endian */
    u8 ADDR_AI       : 1; /*!< 0: Serial interface (SPI or I2C) address do not auto increment.; 1: Serial interface (SPI or I2C) address auto increment */
    u8 SIM           : 1; /*!< 0: Enables 4-wire SPI interface; 1: Enables 3-wire SPI interface */
  };
#elif (defined QMI8658C)
  struct{
    u8 SensorDisable : 1; /*!< 0: Enables internal 2 MHz oscillator; 1: Disables internal 2 MHz oscillator */
    u8 reserved      : 4;
    u8 BE            : 1; /*!< 0: Serial interface (SPI, I2C, I3C) read data Little-Endian; 1: Serial interface (SPI, I2C, I3C) read data Big-Endian */
    u8 ADDR_AI       : 1; /*!< 0: Serial interface (SPI or I2C) address do not auto increment.; 1: Serial interface (SPI or I2C) address auto increment */
    u8 SIM           : 1; /*!< 0: Enables 4-wire SPI interface; 1: Enables 3-wire SPI interface */
  };
#endif
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

#if (defined QMI8658C)
typedef union{
  struct{
    u8 mODR       : 3;
    u8 mDEV       : 4;
    u8 Reserved   : 1;
  };
  u8 reg;
} tQmi8658RegCTRL4;
#endif

typedef union{
  struct{
    u8 aLPF_EN    : 1;
    u8 aLPF_MODE  : 2;
    u8 Reserved0  : 1;
    u8 gLPF_EN    : 1;
    u8 gLPF_MODE  : 2;
    u8 Reserved1  : 1;
  };
  u8 reg;
} tQmi8658RegCTRL5;

#if (defined QMI8658C)
typedef union{
  struct{
    u8 sODR      : 3; /*!< Attitude Engine Output Data Rate (ODR) */
    u8 Reserved  : 4;
    u8 sMoD      : 1; /*!< 0: Disables Motion on Demand. 1: Enables Motion on Demand (Requires sEN=1). */
  };
  u8 reg;
} tQmi8658RegCTRL6;
#endif

typedef union{
#if (defined QMI8658A)
  struct{
    u8 aEN       : 1;   /*!< 1: Enable Accelerometer */
    u8 gEN       : 1;   /*!< 1: Enable Gyroscope */
    u8 Reserved0 : 2;
    u8 gSN       : 1;   /*!< 0: Gyroscope in Full Mode (Drive and Sense are enabled). 1: Gyroscope in Snooze Mode (only Drive enabled). 
                        This bit is effective only when gEN is set to 1. */
    u8 DRDY_DIS  : 1;   /*!< 0: DRDY(Data Ready) is enabled, is driven to the INT2 pin. 1: DRDY(Data Ready) is disabled, is blocked from the INT2 pin */
    u8 Reserved1 : 1;
    u8 syncSmpl  : 1;   /*!< 0: Disable SyncSample mode; 1: Enable SyncSample mode */
  };
#elif (defined QMI8658C)
  struct{
    u8 aEN       : 1;   /*!<  1: Enable Accelerometer */
    u8 gEN       : 1;   /*!<  1: Enable Gyroscope */
    u8 mEN       : 1;   /*!<  1: Enable Magnetometer */
    u8 sEN       : 1;   /*!<  0: Disable AttitudeEngine orientation and velocity increment computation
                              1: Enable AttitudeEngine orientation and velocity increment computation */
    u8 gSN       : 1;   /*!<  0: Gyroscope in Full Mode (Drive and Sense are enabled). 
                              1: Gyroscope in Snooze Mode (only Drive enabled). 
                              This bit is effective only when gEN is set to 1. */
    u8 Reserved  : 1;
    u8 sys_hs    : 1;
    u8 syncSmpl  : 1;   /*!< 0: Disable SyncSample mode; 1: Enable SyncSample mode */
  };
#endif
  u8 reg;
} tQmi8658RegCTRL7;

typedef union{
#if (defined QMI8658A)
  struct{
    u8 Tap_EN               : 1;
    u8 AnyMotion_EN         : 1;
    u8 NoMotion_EN          : 1;
    u8 SigMotion_EN         : 1;
    u8 Pedo_EN              : 1;
    u8 reserved             : 1;
    u8 ACTIVITY_INT_SEL     : 1; /*!< 0: INT2 is used for Activity Detection event interrupt; 1: INT1 is used for Activity Detection event interrupt */
    u8 CTRL9_HandShake_Type : 1; /*!< 0: use INT1 as CTRL9 handshake; 1: use STATUSINT.bit7 as CTRL9 handshake */
  };
#elif (defined QMI8658C)
  struct{
    u8 reserved : 8;
  };
#endif
  u8 reg;
} tQmi8658RegCTRL8;

/**
 * @note
 *  Configure the FIFO_CTRL.FIFO_MODE to ‘FIFO’(1) or ‘Stream’(2) mode will enable the FIFO functionality. 
 *  Configure the FIFO_MODE to ‘Bypass’ (0) mode, will disable the FIFO functionality.
 * 
 *  In ‘Stream’ mode, once FIFO is full, the data filling will continue and the oldest data will be discarded, 
 *  until host reads out the FIFO data and release the space for new data to be written to.
 * 
 *  In ‘FIFO’ mode, once FIFO is full, the data filling will stop and new data will be discarded until host reads 
 *  out the FIFO data and release the space for new data to be written to.
 * 
 *  There are four levels of FIFO size: 16 samples, 32 samples, 64 samples, 128 samples. The sample stands for 
 *  6 bytes of accelerometer data or 6 bytes of gyroscope data if one of them is enabled, or 6 bytes of 
 *  accelerometer and 6 bytes of gyroscope data (total 12 bytes) if both are enabled.
 * 
 * @attention
 *  When the FIFO is enabled for two sensors (Accelerometer and Gyroscope), the sensors must be set at the same 
 *  Output Data Rate (ODR), refer to Table 22 for CTRL2 and CTRL3 registers.
 */
typedef union{
  struct{
    u8 FIFO_MODE    :2;     /*!< 00: Bypass; 01: FIFO; 10: Stream; 11: Reserved */
    u8 FIFO_SIZE    :2;     /*!< 00: 16 samples; 01: 32 samples; 64 samples; 128 samples; */
    u8 Reserved     :3;
    u8 FIFO_RD_MODE :1;
  };
  u8 reg;
} tQmi8658RegFIFO_CTRL;

typedef union{
  struct{
    u8 FIFO_SMPL_CNT_MSB : 2;
    u8 Reserved          : 2;
    u8 FIFO_NOT_EMPTY    : 1;
    u8 FIFO_OVFLOW       : 1;
    u8 FIFO_WTM          : 1;
    u8 FIFO_FULL         : 1;
  };
  u8 reg;
} tQmi8658RegFIFO_STATUS;


/* ************************************************************************** */
/*                             Private Functions                              */
/* ************************************************************************** */
STATIC INLINE tBspGyroAccSensitivity    bsp_qmi8658_acc_scale_2_sensitivity  ( tQmi8658AccScale scale);
STATIC INLINE tBspGyroDegreeSensitivity bsp_qmi8658_gyro_scale_2_sensitivity ( tQmi8658GyroScale scale);
STATIC cmnBoolean_t                     bsp_qmi8658_i2c_polling_send         ( u8 reg, const u8 *buf, u8 len);
STATIC cmnBoolean_t                     bsp_qmi8658_i2c_polling_recv         ( u8 reg, u8 *buf, u8 len);
#if 0 /* Currently no requirement for dma tx */
STATIC cmnBoolean_t                     bsp_qmi8658_i2c_dma_send             ( u8 reg, const u8 *buf, u8 len);
#endif
STATIC cmnBoolean_t                     bsp_qmi8658_i2c_dma_recv             ( u8 reg, u8 *buf, u8 len);
STATIC u8                               bsp_qmi8658_i2c_polling_recv_1byte   ( u8 reg);
STATIC void                             bsp_qmi8658_i2c_polling_send_1byte   ( u8 reg, u8 val);
STATIC INLINE cmnBoolean_t              bsp_qmi8658_is_ready                 (void);
STATIC void                             bsp_qmi8658_set_acc                  ( tQmi8658AccOdr odr, tQmi8658AccScale scale);
STATIC void                             bsp_qmi8658_set_gyro                 ( tQmi8658GyroOdr odr, tQmi8658GyroScale scale);
STATIC tBspGyroAccSensitivity           bsp_qmi8658_get_acc_scale            (void);
STATIC tBspGyroDegreeSensitivity        bsp_qmi8658_get_gyro_scale           (void);
STATIC INLINE  cmnBoolean_t             bsp_qmi8658_ctrl9_cmddone_ack        (void);
STATIC cmnBoolean_t                     bsp_qmi8658_ctrl9_protocol           ( u8 QMI8658_CTRL9_CMD_XXXX);



/* ************************************************************************** */
/*                             Public Functions                               */
/* ************************************************************************** */
/**
 * @brief Turn ON/OFF the device. (Power Switch)
 * @param [in] on_off - Switch
 * @addtogroup MachineDependent
 */
void bsp_qmi8658_switch( cmnBoolean_t on_off){
  HAL_GPIO_WritePin( GYRO_EN_GPIO_Port, GYRO_EN_Pin, on_off!=OFF);
}


/**
 * @brief Software Reset Chip
 * @note  Register Address: 0x60 [REG RESET]
 */
void bsp_qmi8658_reset( void){
  u8 data = QMI8658_REG_RESET_DEFAULT;
  bsp_qmi8658_i2c_polling_send( QMI8658_REG_RESET, &data, sizeof(data));

  /* Wait until an interrupt occurred */
  while(metope.dev.status.B5==0);
  tQmi8658RegSTATUS1 reg_status1 = {0};
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_STATUS1, &reg_status1.reg, 1);
  metope.dev.status.B5 = 0;
}


/**
 * @brief QMI8658 Device Initialization
 * @note  Accelerometer Output Rate: 8000Hz
 * @note  Accelerometer Full Scale: 2g
 * @note  Gyroscope Output Rate: 8000Hz
 * @note  Gyroscope Full Scale: 16 degree per second
 * @return `SUCCESS` | `ERROR`
 */
cmnBoolean_t bsp_qmi8658_init(void){
  bsp_qmi8658_switch(ON);
  bsp_qmi8658_reset();
  cmnBoolean_t async_mode = metope.app.rtos.status.running==true;
  cmn_tim2_sleep(2000, async_mode);
  
  /**
   * @note
   *  - Auto Address Increment
   *  - Big Endian
   */
  {
    tQmi8658RegCTRL1 reg_ctrl1 = {
      .ADDR_AI       = 1,
      .BE            = 1,
      .SensorDisable = 0,
    };
    bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL1, &reg_ctrl1.reg, 1);
  }

  /**
   * @note
   *  - Full Scale: Default
   *  - Output Rate: Default
   */
  {
    tQmi8658RegCTRL2 reg_ctrl2 = {
      .aFS  = QMI8658_DEFAULT_ACC_SCALE,
      .aODR = QMI8658_DEFAULT_ACC_ODR,
    };
    bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL2, &reg_ctrl2.reg, 1);
  }

  /**
   * @note
   *  - Full Scale: Default
   *  - Output Rate: Default
   */
  {
    tQmi8658RegCTRL3 reg_ctrl3 = {
      .gFS  = QMI8658_DEFAULT_GYRO_SCALE,
      .gODR = QMI8658_DEFAULT_GYRO_ODR,
    };
    bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL3, &reg_ctrl3.reg, 1);
  }

  /**
   * @note
   *  - LPF: Enable for a & g
   *  - BW: 5.39% of ODR
   */
  {
    tQmi8658RegCTRL5 reg_ctrl5 = {
      .aLPF_EN   = 1,
      .aLPF_MODE = 2,
      .gLPF_EN   = 1,
      .gLPF_MODE = 2
    };
    bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL5, &reg_ctrl5.reg, 1);
  }

  /**
   * @note
   *  - Enable a & g
   *  - Data Ready mapped to INT2
   */
  {
    tQmi8658RegCTRL7 reg_ctrl7 = {
      .DRDY_DIS = 1,
      .gEN      = 1,
      .aEN      = 1
    };
    bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL7, &reg_ctrl7.reg, 1);
  }

  if( QMI8658_REG_WHOAMI_DEFAULT != bsp_qmi8658_get_who_am_i()){
    return ERROR;
  }else{
    return SUCCESS;
  }
}


/**
 * @brief  Chip Revision ID
 * @note   Register Address: 0x01
 * @return Return Revision ID
 */
u8 bsp_qmi8658_get_chip_id(void){
  u8 data = 0x00;
  if(ERROR==bsp_qmi8658_i2c_polling_recv(QMI8658_REG_REVISION, &data, 1)){
    data = 0xFF;
  }
  return data;
}


/**
 * @brief  Chip Who Am I
 * @note   Register Address: 0x00
 * @return Return Who Am I value. Always 0x05.
 */
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
  if(false==bsp_qmi8658_is_ready()){
    return ERROR;
  }
  data->acc_sensitivity = bsp_qmi8658_get_acc_scale();
  data->deg_sensitivity = bsp_qmi8658_get_gyro_scale();

#if 0
  u8 tmp[14];
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_TEMPERATURE_L, &tmp[0], 14);

  // read accelerometer data
  data->acc.x = (((int16_t)tmp[QMI8658_REG_AX_H-QMI8658_REG_TEMPERATURE_L] << 8) | tmp[QMI8658_REG_AX_L-QMI8658_REG_TEMPERATURE_L]);
  data->acc.y = (((int16_t)tmp[QMI8658_REG_AY_H-QMI8658_REG_TEMPERATURE_L] << 8) | tmp[QMI8658_REG_AY_L-QMI8658_REG_TEMPERATURE_L]);
  data->acc.z = (((int16_t)tmp[QMI8658_REG_AZ_H-QMI8658_REG_TEMPERATURE_L] << 8) | tmp[QMI8658_REG_AZ_L-QMI8658_REG_TEMPERATURE_L]);
  data->acc_sensitivity = bsp_qmi8658_get_acc_scale();

  // read gyroscope data
  data->gyro.x = (((int16_t)tmp[QMI8658_REG_GX_H-QMI8658_REG_TEMPERATURE_L] << 8) | tmp[QMI8658_REG_GX_L-QMI8658_REG_TEMPERATURE_L]);
  data->gyro.y = (((int16_t)tmp[QMI8658_REG_GY_H-QMI8658_REG_TEMPERATURE_L] << 8) | tmp[QMI8658_REG_GY_L-QMI8658_REG_TEMPERATURE_L]);
  data->gyro.z = (((int16_t)tmp[QMI8658_REG_GZ_H-QMI8658_REG_TEMPERATURE_L] << 8) | tmp[QMI8658_REG_GZ_L-QMI8658_REG_TEMPERATURE_L]);
  data->deg_sensitivity = bsp_qmi8658_get_gyro_scale();

  // read temperature data
  data->temperature = (((int16_t)tmp[QMI8658_REG_TEMPERATURE_H-QMI8658_REG_TEMPERATURE_L] << 8) | tmp[QMI8658_REG_TEMPERATURE_L-QMI8658_REG_TEMPERATURE_L]);
#endif

  return bsp_qmi8658_i2c_polling_recv( QMI8658_REG_TEMPERATURE_L, &data->raw[0], sizeof(data->raw));
}


/**
 * 
 * @return `SUCCESS` | `ERROR`
 */
cmnBoolean_t bsp_qmi8658_fifo_set_watermark(u8 nSample){
  return bsp_qmi8658_i2c_polling_send( QMI8658_REG_FIFO_WTM_TH, &nSample, 1);
}


/**
 * @brief Get the content level of FIFO
 * @note
 *  FIFO_Sample_Count (in byte) = 2 * (fifo_smpl_cnt_msb[1:0] * 256 + fifo_smpl_cnt_lsb[7:0])
 * @return Return FIFO Sample Count in bytes
 */
u16 bsp_qmi8658_fifo_get_sample_cnt(void){
  u8 FIFO_SMPL_CNT_LSB;
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_FIFO_COUNT, &FIFO_SMPL_CNT_LSB, 1);

  tQmi8658RegFIFO_STATUS reg_fifo_status;
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_FIFO_STATUS, &reg_fifo_status.reg, 1);

  return (u16)(((reg_fifo_status.FIFO_SMPL_CNT_MSB<<8) | FIFO_SMPL_CNT_LSB)<<1);
}


/**
 * @note
 *  FIFO Output Data Rate: 2KHz | Size: 128 Sample | Watermark: 64 Sample | Acc 2g | Gyro 2048 dps
 */
cmnBoolean_t bsp_qmi8658_fifo_enable(void){
  u8 ret = SUCCESS;

  /* Disable the sensor */
  tQmi8658RegCTRL7 reg_ctrl7;
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_CTRL7, &reg_ctrl7.reg, 1);
  reg_ctrl7.aEN      = 0;
  reg_ctrl7.gEN      = 0;
  bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL7, &reg_ctrl7.reg, 1);

  /* Config Acc & Gyro */
  bsp_qmi8658_set_acc(kAccOdr_2000Hz, kAccScale_2g);
  bsp_qmi8658_set_gyro(kGyroOdr_2000Hz, kGyroScale_2048dps);

  /* Reset FIFO */
  ret |= bsp_qmi8658_ctrl9_protocol(QMI8658_CTRL9_CMD_RST_FIFO);

  /* Config Fifo */
  tQmi8658RegFIFO_CTRL reg_fifo_ctrl = {
    .FIFO_RD_MODE = 0, // Write Mode
    .FIFO_SIZE    = 3, // 128 Samples
    .FIFO_MODE    = 2, // Stream Mode
  };
  bsp_qmi8658_i2c_polling_send( QMI8658_REG_FIFO_CTRL, &reg_fifo_ctrl.reg, 1);

  ret |= bsp_qmi8658_fifo_set_watermark(64);

  /* Setup Interrupt */
  /**
   * @note: INT1:=Ctrl9CmdDone; INT2:=FIFO
   */
  tQmi8658RegCTRL1 reg_ctrl1;
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_CTRL1, &reg_ctrl1.reg, 1);
  reg_ctrl1.INT1_EN = 1;
  reg_ctrl1.INT2_EN = 1;
  reg_ctrl1.FIFO_INT_SEL = 0;
  bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL1, &reg_ctrl1.reg, 1);
  
  tQmi8658RegCTRL8 reg_ctrl8;
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_CTRL8, &reg_ctrl8.reg, 1);
  reg_ctrl8.CTRL9_HandShake_Type = 0;
  reg_ctrl8.ACTIVITY_INT_SEL     = 0;
  bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL8, &reg_ctrl8.reg, 1);
  metope.dev.status.B5 = 0; // Clear system interrupt bit
  metope.dev.status.B6 = 0; // Clear system interrupt bit

  /* Enable the sensor */
  reg_ctrl7.aEN = 1;
  reg_ctrl7.gEN = 1;
  reg_ctrl7.DRDY_DIS = 1;
  reg_ctrl7.syncSmpl = 0;
  bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL7, &reg_ctrl7.reg, 1);

  tQmi8658RegFIFO_STATUS reg_fifo_status;
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_FIFO_STATUS, &reg_fifo_status.reg, 1);
  
  volatile u8 reg_fifo_samp_cnt;
  reg_fifo_samp_cnt = bsp_qmi8658_fifo_get_sample_cnt();
  UNUSED(reg_fifo_samp_cnt);
  
  ret |= bsp_qmi8658_ctrl9_protocol(QMI8658_CTRL9_CMD_REQ_FIFO);

  /* Adjust I2C DMA speed below FIFO ODR */

  return ret;
}


/**
 * @brief Keep updating the data
 * @attention Buffer `data` MUST be valid until `bsp_qmi8658_stop_updating()` is called
 * @param [in] data - Buffer pointer
 * @return 
 */
cmnBoolean_t bsp_qmi8658_dma_update( tBspGyroData *data){
  if(false==bsp_qmi8658_is_ready()){
    return ERROR;
  }

  data->acc_sensitivity = bsp_qmi8658_acc_scale_2_sensitivity(QMI8658_DEFAULT_ACC_SCALE);
  data->deg_sensitivity = bsp_qmi8658_gyro_scale_2_sensitivity(QMI8658_DEFAULT_GYRO_SCALE);

  if(metope.app.rtos.status.running){
    //...//
  }

  return bsp_qmi8658_i2c_dma_recv( QMI8658_REG_TEMPERATURE_L , &data->raw[0], sizeof(data->raw));
}



/* ************************************************************************** */
/*                             Private Functions                              */
/* ************************************************************************** */
/**
 * @brief Convert Register Value to Mathematical one
 * @param [in] scale - Register Value
 * @return Return Mathematical Value
 */
STATIC INLINE tBspGyroAccSensitivity bsp_qmi8658_acc_scale_2_sensitivity( tQmi8658AccScale scale){
  return (enum tBspGyroAccSensitivity)(1<<(14-scale));
}

/**
 * @brief Convert Register Value to Mathematical one
 * @param [in] scale - Register Value
 * @return Return Mathematical Value
 */
STATIC INLINE tBspGyroDegreeSensitivity bsp_qmi8658_gyro_scale_2_sensitivity( tQmi8658GyroScale scale){
  return (enum tBspGyroDegreeSensitivity)(1<<(11-scale));
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

#if 0 /* Currently no requirement for dma tx */
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
  if(metope.app.rtos.status.running){
    xEventGroupWaitBits( metope.app.rtos.event._handle, CMN_EVENT_QMI8658_TX_CPLT, pdTRUE, pdFALSE, portMAX_DELAY);
  }else{
    while(IDLE!=metope.dev.status.i2c1);
  }

  return SUCCESS;
}
#endif

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
  
  // if(metope.app.rtos.status.running){
  //   xEventGroupWaitBits( metope.app.rtos.event._handle, CMN_EVENT_QMI8658_TX_CPLT, pdTRUE, pdFALSE, portMAX_DELAY);
  // }else{
  //   while(IDLE!=metope.dev.status.i2c1);
  // }

  return SUCCESS;
}

/**
 * @brief Read 1 byte from register
 */
STATIC u8 bsp_qmi8658_i2c_polling_recv_1byte( u8 reg){
  u8 data;
  bsp_qmi8658_i2c_polling_recv( reg, &data, 1);
  return data;
}

/**
 * @brief Read 1 byte from register
 */
STATIC void bsp_qmi8658_i2c_polling_send_1byte( u8 reg, u8 val){
  bsp_qmi8658_i2c_polling_send( reg, &val, 1);
}

/**
 * @brief Check if data is available
 * @return Return `true` => Ready | Return `false` => N/A
 */
STATIC INLINE cmnBoolean_t bsp_qmi8658_is_ready(void){
  tQmi8658RegSTATUS0 reg_status0;
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_STATUS0, &reg_status0.reg, 1);
  return reg_status0.aDA&&reg_status0.gDA;
}

/**
 * @brief Config Accelerometer Output Data Rate & Scale Sensitivity
 * @note  Register Address: 0x03 [REG CTRL2]
 * @param [in] odr   Output Data Rate
 * @param [in] scale Scale Sensitivity
 */
STATIC void bsp_qmi8658_set_acc( tQmi8658AccOdr odr, tQmi8658AccScale scale){
  tQmi8658RegCTRL2 reg_ctrl2;
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_CTRL2, &reg_ctrl2.reg, 1);
  reg_ctrl2.aODR = odr;
  reg_ctrl2.aFS  = scale;
  bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL2, &reg_ctrl2.reg, 1);
}

/**
 * @brief Config Gyroscope(Angular Rate) Output Data Rate & Scale Sensitivity
 * @note  Register Address: 0x04 [REG CTRL3]
 * @param [in] odr   Output Data Rate
 * @param [in] scale Scale Sensitivity
 */
STATIC void bsp_qmi8658_set_gyro(tQmi8658GyroOdr odr, tQmi8658GyroScale scale){
  tQmi8658RegCTRL3 reg_ctrl3;
  bsp_qmi8658_i2c_polling_recv( QMI8658_REG_CTRL3, &reg_ctrl3.reg, 1);
  reg_ctrl3.gODR = odr;
  reg_ctrl3.gFS  = scale;
  bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL3, &reg_ctrl3.reg, 1);
}

/**
 * @brief  Get Gyroscope(Angular Rate) Scale Sensitivity
 * @note   Register Address: 0x03 [REG CTRL2]
 *         The enumeration value is mathematically valid
 * @return Return Gyroscope(Angular Rate) Scale Sensitivity
 */
STATIC enum tBspGyroAccSensitivity bsp_qmi8658_get_acc_scale(void){
  tQmi8658RegCTRL2 reg_ctrl2 = {.reg=bsp_qmi8658_i2c_polling_recv_1byte(QMI8658_REG_CTRL2)};
  // 0 -> 1<<14
  // 1 -> 1<<13
  // 2 -> 1<<12
  // 3 -> 1<<11
  return bsp_qmi8658_acc_scale_2_sensitivity((tQmi8658AccScale)reg_ctrl2.aFS);
}

/**
 * @brief  Get Gyroscope(Angular Rate) Scale Sensitivity
 * @note   Register Address: 0x04 [REG CTRL3]
 *         The enumeration value is mathematically valid
 * @return Return Gyroscope(Angular Rate) Scale Sensitivity
 */
STATIC enum tBspGyroDegreeSensitivity bsp_qmi8658_get_gyro_scale(void){
  tQmi8658RegCTRL3 reg_ctrl3 = {.reg=bsp_qmi8658_i2c_polling_recv_1byte(QMI8658_REG_CTRL3)};
  // 0 -> 1<<11
  // 1 -> 1<<10
  // 2 -> 1<< 9
  // ...
  // 7 -> 1<< 4
  return bsp_qmi8658_gyro_scale_2_sensitivity((tQmi8658GyroScale)reg_ctrl3.gFS);
}

/**
 * @brief  CTRL9 Register Protocol Ack
 * @return `SUCCESS` | `ERROR`
 */
STATIC INLINE cmnBoolean_t bsp_qmi8658_ctrl9_cmddone_ack(void){
  u8 data = 0x00;
  return bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL9, &data, 1);
}

/**
 * @brief Communicate with CTRL9 register using chip protocol
 * @attention
 *  This function assumes CTRL9 command done was mapped to INT1.
 * @ref Figure 13. | Table 28.
 * @param [in] QMI8658_CTRL9_CMD_XXXX - CTRL9 command. Check Table 28.
 * @return `SUCCESS` | `ERROR`
 */
STATIC cmnBoolean_t bsp_qmi8658_ctrl9_protocol(u8 QMI8658_CTRL9_CMD_XXXX){
  metope.dev.status.B5 = 0;
  switch(QMI8658_CTRL9_CMD_XXXX){
    /* No data transaction is required prior to or following the Ctrl9 protocol. */
    case QMI8658_CTRL9_CMD_RST_FIFO:{
      bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL9, &QMI8658_CTRL9_CMD_XXXX, 1);
      break;
    }

    /* The host gets data from QMI8658A following the Ctrl9 protocol. (Ctrl9 protocol – Read) */
    case QMI8658_CTRL9_CMD_REQ_FIFO:{
      bsp_qmi8658_i2c_polling_send( QMI8658_REG_CTRL9, &QMI8658_CTRL9_CMD_XXXX, 1);
      break;
    }

    /* The host needs to supply data to QMI8658A prior to the Ctrl9 protocol. (Write – Ctrl9 Protocol) */
    
    default:{
      return ERROR;
    }
  }
  
  if(metope.app.rtos.status.running){
    /* Process Blocked */
    xEventGroupWaitBits( metope.app.rtos.event._handle, CMN_EVENT_QMI8658_INT1, pdTRUE, pdFALSE, portMAX_DELAY);
  }else{
    /* Loop Checking */
    while(metope.dev.status.B5==0);
  }

  bsp_qmi8658_i2c_polling_send_1byte( QMI8658_REG_CTRL9, QMI8658_CTRL9_CMD_ACK);

  u16 timeout = 100U;
  tQmi8658RegSTATUSINT reg_statusint;
  while(timeout--){
    bsp_qmi8658_i2c_polling_recv( QMI8658_REG_STATUS_INT, &reg_statusint.reg, 1);
    /* Check if the cmd done was cleared */
    if(reg_statusint.CmdDone==0){
      break;
    }
    cmnBoolean_t async_mode = metope.app.rtos.status.running==true;
    cmn_tim2_sleep( 1, async_mode);
  }
  if(0==timeout){
    bsp_qmi8658_i2c_polling_recv( QMI8658_REG_STATUS_INT, &reg_statusint.reg, 1);
    if(reg_statusint.CmdDone!=0){
      return ERROR;
    }
  }

  return SUCCESS;
}




/* ************************************************************************** */
/*                            Debugging Functions                             */
/* ************************************************************************** */
#if (defined TEST_ONLY) && (TEST_ONLY==1)
u8 bsp_qmi8658_debug_read_reg(u8 reg){
  return bsp_qmi8658_i2c_polling_recv_1byte(reg);
}

void bsp_qmi8658_debug_write_reg(u8 reg, u8 val){
  bsp_qmi8658_i2c_polling_send_1byte(reg, val);
}

#endif

#ifdef __cplusplus
}
#endif