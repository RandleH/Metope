/**
 ******************************************************************************
 * @file    bsp_gyro.h
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

/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include "cmn_type.h"

/* ************************************************************************** */
/*                              Headfile Guards                               */
/* ************************************************************************** */
#ifndef BSP_GYRO_H
#define BSP_GYRO_H

#ifdef __cplusplus
extern "C"{
#endif


/* ************************************************************************** */
/*                                    Types                                   */
/* ************************************************************************** */
/* Accelerometer sensitivity table */
typedef enum tBspGyroAccSensitivity{
  ACC_SCALE_SENSITIVITY_2G  = (1 << 14),   // Sensitivity for ±2g range.
  ACC_SCALE_SENSITIVITY_4G  = (1 << 13),   // Sensitivity for ±4g range.
  ACC_SCALE_SENSITIVITY_8G  = (1 << 12),   // Sensitivity for ±8g range.
  ACC_SCALE_SENSITIVITY_16G = (1 << 11),   // Sensitivity for ±16g range.
  ACC_SCALE_SENSITIVITY_INVALID = (1<<15)
} tBspGyroAccSensitivity;
                                  
/* Gyroscope sensitivity table */
typedef enum tBspGyroDegreeSensitivity{
  GYRO_SCALE_SENSITIVITY_16DPS   = (1 << 11),     // Sensitivity for ±16 degrees per second range.
  GYRO_SCALE_SENSITIVITY_32DPS   = (1 << 10),     // Sensitivity for ±32 degrees per second range.
  GYRO_SCALE_SENSITIVITY_64DPS   = (1 << 9 ),     // Sensitivity for ±64 degrees per second range.
  GYRO_SCALE_SENSITIVITY_128DPS  = (1 << 8 ),     // Sensitivity for ±128 degrees per second range.
  GYRO_SCALE_SENSITIVITY_256DPS  = (1 << 7 ),     // Sensitivity for ±256 degrees per second range.
  GYRO_SCALE_SENSITIVITY_512DPS  = (1 << 6 ),     // Sensitivity for ±512 degrees per second range.
  GYRO_SCALE_SENSITIVITY_1024DPS = (1 << 5 ),     // Sensitivity for ±1024 degrees per second range.
  GYRO_SCALE_SENSITIVITY_2048DPS = (1 << 4 )      // Sensitivity for ±2048 degrees per second range.
} tBspGyroDegreeSensitivity;

typedef union{
  /* QMI8658 Full Output Buffer */
  struct{
    u8 raw[14];
  };

  /* QMI8658 FIFO Pattern Buffer */
  struct{
    u8 fifo[12];
  };

  /* QMI8658 Raw Sensor Data */
  struct{
    /**
     * @note: T = temp_H + temp_L /  [ TEMPERATURE_SENSOR_RESOLUTION:=256 ]
     */
    union{
      u8  temp_L;
      u8  temp_H;
      i16 temperature;
    };

/**
 * @attention
 *  Based on my observation, the address for Acceleration and Angular Rate are inverted.
 *  Try both to verify which one is correct. Do NOT fully trust the datasheet.
 */
#if 0
    /**
     * @note: Ax = acc.x / acc_sensitivity; ...
     */
    struct{
      i16 x;
      i16 y;
      i16 z;
    }acc;
    
    /**
     * @note: Gx = gyro.x / deg_sensitivity; ...
     */
    struct{
      i16 x;
      i16 y;
      i16 z;
    }gyro;
#else
    /**
     * @note: Ax = acc.x / acc_sensitivity; ...
     */
    struct{
      i16 x;
      i16 y;
      i16 z;
    }gyro;
    
    /**
     * @note: Gx = gyro.x / deg_sensitivity; ...
     */
    struct{
      i16 x;
      i16 y;
      i16 z;
    }acc;
#endif

    enum tBspGyroAccSensitivity    acc_sensitivity;
    enum tBspGyroDegreeSensitivity deg_sensitivity;
  };
  
} tBspGyroData;


/* ************************************************************************** */
/*                             Public Functions                               */
/* ************************************************************************** */
void         bsp_qmi8658_reset        (void);
cmnBoolean_t bsp_qmi8658_init         (void);
u8           bsp_qmi8658_get_chip_id  (void);
u8           bsp_qmi8658_get_who_am_i (void);
void         bsp_qmi8658_switch       ( cmnBoolean_t on_off);
cmnBoolean_t bsp_qmi8658_update       ( tBspGyroData *data);
cmnBoolean_t bsp_qmi8658_fifo_enable  (void);
cmnBoolean_t bsp_qmi8658_dma_update   ( tBspGyroData *data);

/* ************************************************************************** */
/*                            Debugging Functions                             */
/* ************************************************************************** */
#if (defined TEST_ONLY) && (TEST_ONLY==1)
u8   bsp_qmi8658_debug_read_reg  ( u8 reg);
void bsp_qmi8658_debug_write_reg ( u8 reg, u8 val);
#endif

#ifdef __cplusplus
}
#endif


#endif
