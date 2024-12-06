/**
 ******************************************************************************
 * @file    bsp_rtc.c
 * @author  RandleH
 * @brief   Board Support Package Delivery - Real Time Clock Module (PCF8563)
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

#include "bsp_rtc.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_dma.h"

#include "global.h"






#define PCF8563_ADDRESS               (0x51<<1)
#define PCF8563_I2C_TIMEOUT           100

#define PCF8563_REG_CONTROL_STATUS1   0x00
#define PCF8563_REG_CONTROL_STATUS2   0x01
#define PCF8563_REG_TIME              0x02
#define PCF8563_REG_SECONDS           0x02
#define PCF8563_REG_MINUTES           0x03
#define PCF8563_REG_HOURS             0x04
#define PCF8563_REG_DATE              0x05
#define PCF8563_REG_WEEKDAY           0x06
#define PCF8563_REG_MONTH             0x07
#define PCF8563_REG_YEAR              0x08
#define PCF8563_REG_ALARM_MINUTE      0x09
#define PCF8563_REG_ALARM_HOUR        0x0A
#define PCF8563_REG_ALARM_DAY         0x0B
#define PCF8563_REG_ALARM_WEEKDAY     0x0C
#define PCF8563_REG_CLKOUT            0x0D
#define PCF8563_REG_TIMER_CONTROL     0x0E
#define PCF8563_REG_TIMER             0x0F

//
//    Controll register 1 0x00
//
#define PCF8563_CONTROL1_TEST1    7
#define PCF8563_CONTROL1_STOP     5
#define PCF8563_CONTROL1_TESTC    3

//
//    Controll register 2 0x01
//
#define PCF8563_CONTROL2_TI_TP    4
#define PCF8563_CONTROL2_AF       3
#define PCF8563_CONTROL2_TF       2
#define PCF8563_CONTROL2_AIE      1
#define PCF8563_CONTROL2_TIE      0

//
//    CLKOUT control register 0x0D
//
#define PCF8563_CLKOUT_CONTROL_FE      7
#define PCF8563_CLKOUT_CONTROL_FD1     1
#define PCF8563_CLKOUT_CONTROL_FD0     0


#ifdef __cplusplus
extern "C"{
#endif


typedef enum
{
  CLKOUT_FREQ_1HZ     = 3,
  CLKOUT_FREQ_32HZ    = 2,
  CLKOUT_FREQ_1024HZ  = 1,
  CLKOUT_FREQ_32768HZ = 0
}CLKOUT_Freq;

typedef struct
{
  uint16_t Year;
  uint8_t  Month;
  uint8_t  Day;
  uint8_t  Hour;
  uint8_t  Minute;
  uint8_t  Second;
  uint8_t  DayOfWeek;
}RTCDateTime;


/**
 * @addtogroup MachineDependent
 */
static void bsp_rtc_write_reg_bit( u8 reg, u8 bit_pos, u8 bit_len, u8 bit_val){
  uint8_t tmp;
  HAL_I2C_Mem_Read( &hi2c2, PCF8563_ADDRESS, reg, 1, &tmp, 1, PCF8563_I2C_TIMEOUT);
	tmp &= ~(((1<<bit_len)-1)<<bit_pos);
	tmp |= ( ((u8)bit_val)<<bit_pos);

  if( reg == PCF8563_REG_CONTROL_STATUS1){
    tmp &= 0b00011111;
  }else if( reg == PCF8563_REG_CONTROL_STATUS2){
    tmp &= 0b10101000;
  }

	HAL_I2C_Mem_Write( &hi2c2, PCF8563_ADDRESS, reg, 1, &tmp, 1, PCF8563_I2C_TIMEOUT);
}

/**
 * @addtogroup MachineDependent
 */
static void bsp_rtc_read_reg( u8 reg, u8 *buf, u8 len){
  HAL_I2C_Mem_Read( &hi2c2, PCF8563_ADDRESS, reg, 1, buf, len, PCF8563_I2C_TIMEOUT);
}

/**
 * @addtogroup MachineDependent
 */
static void bsp_rtc_read_reg_dma( u8 reg, u8 *buf, u8 len){
  HAL_I2C_Mem_Read_DMA( &hi2c2, PCF8563_ADDRESS, reg, 1, buf, len);
}

/**
 * @addtogroup MachineDependent
 */
static void bsp_rtc_write_reg( u8 reg, const u8 *buf, u8 len){
  HAL_I2C_Mem_Write( &hi2c2, PCF8563_ADDRESS, reg, 1, (u8*)buf, len, PCF8563_I2C_TIMEOUT);
}

/**
 * @addtogroup MachineDependent
 */
static void bsp_rtc_write_reg_dma( u8 reg, const u8 *buf, u8 len){
  HAL_I2C_Mem_Write_DMA( &hi2c2, PCF8563_ADDRESS, reg, 1, (u8*)buf, len);
}


static uint8_t bcd2dec(uint8_t BCD)
{
	return (((BCD & 0xF0)>>4) *10) + (BCD & 0xF);
}

static uint8_t dec2bcd(uint8_t DEC)
{
	return ((DEC / 10)<<4) + (DEC % 10);
}

int dayofweek(int Day, int Month, int Year)
{
  int Y, C, M, N, D;
  M = 1 + (9 + Month) % 12;
  Y = Year - (M > 10);
  C = Y / 100;
  D = Y % 100;
  N = ((13 * M - 1) / 5 + D + D / 4 + 6 * C + Day + 5) % 7;
  return (7 + N) % 7;
}



void bsp_rtc_set_inputfreq(CLKOUT_Freq Frequency)
{
	bsp_rtc_write_reg_bit( PCF8563_REG_CLKOUT, PCF8563_CLKOUT_CONTROL_FD0, 2, (u8)Frequency);
}



void bsp_rtc_switch( u8 on_off){
  bsp_rtc_write_reg_bit(PCF8563_REG_CONTROL_STATUS1, PCF8563_CONTROL1_STOP, 1, (u8)(on_off==0));  // Write 1=STOP; 0=RUN;
}


/**
 * @brief Initialize the RTC module. Chip: PCF8563
 * @note  Crystal Clock Frequency: 32.768KHz
 */
void bsp_rtc_init( void){
  bsp_rtc_set_inputfreq(CLKOUT_FREQ_32768HZ);
  bsp_rtc_switch(ON);
}


cmnDateTime_t bsp_rtc_get_time( void){
  u8 tmp[7];
  cmnDateTime_t result;
  bsp_rtc_read_reg( PCF8563_REG_TIME, tmp, 7);
  
  result.second = bcd2dec(tmp[0] & 0x7F);
  result.minute = bcd2dec(tmp[1]);
  result.hour   = bcd2dec(tmp[2]);
  result.day    = tmp[3];
  result.month  = bcd2dec(tmp[5] & 0x1F);
  result.year   = 2000+bcd2dec(tmp[6])-2024;

  // DayOfWeek = bcd2dec(tmp[4] + 1);
  
  return result;
}


void bsp_rtc_set_time( const cmnDateTime_t t){
  u8 tmp[7];
  tmp[0] = dec2bcd((t.second) & 0x7F);
	tmp[1] = dec2bcd(t.minute);
	tmp[2] = dec2bcd(t.hour);
	tmp[3] = dec2bcd(t.day);
	tmp[4] = dayofweek(t.day, t.month, 2024+t.year);
	tmp[5] = dec2bcd(t.month);
	tmp[6] = dec2bcd(2024+t.year -2000);
  
  bsp_rtc_write_reg( PCF8563_REG_TIME, tmp, 7);
}




#ifdef __cplusplus
}
#endif



