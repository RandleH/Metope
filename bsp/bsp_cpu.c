/**
 ******************************************************************************
 * @file    bsp_cpu.c
 * @author  RandleH
 * @brief   Board Support Package Delivery - CPU
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
#include "device.h"
#include "bsp_cpu.h"


#ifdef __cplusplus
extern "C"{
#endif


/**
 * @brief Set CPU Clock
 * @note  
 *  @cpu: CortexM4 STM32F411CEU6 96MHz
 *  @timer: SYSTICK=84MHz APB1=96MHz APB2=96MHz 
 *  @periphral: AHB=96MHz APB1=48MHz APB2=96MHz USB=48MHz HSE=25MHz LSE=32.768KHz
 * 
 *  @cpu: CortexM4 STM32F405RGT6 84MHz
 *  @timer: SYSTICK=84MHz APB1=84MHz APB2=84MHz 
 *  @periphral: AHB=84MHz APB1=42MHz APB2=84MHz HSE=25MHz LSE=32.768KHz
 * 
 * @addtogroup MachineDependent
 */
void bsp_cpu_clock_init( void){
  /* Set the new HSE configuration */
  SET_BIT(RCC->CR, RCC_CR_HSEON);

#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6)
  /* Wait till HSE is ready */
  while(0==READ_BIT(RCC->CR, RCC_CR_HSERDY));
#endif

  /* Make sure HSI is NOT used as system clock or PLL source */
  // Skipped the checker for memory saving

  MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, RCC_HSICALIBRATION_DEFAULT<<RCC_CR_HSITRIM_Pos);

  /* Make sure PLL is NOT used as system clock*/
  // Skipped the checker for memory saving
  
  /* Disable the main PLL. */
  CLEAR_BIT( RCC->CR, RCC_CR_PLLON ); /*!< Bit-Banding Feature: equivalent to `RCC_CR_PLLON_BB = 0;` */

#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6)
  /* Wait till PLL is disabled (0=unlocked; 1=locked) */
  while(1==READ_BIT(RCC->CR, RCC_CR_PLLRDY));
#endif

#if (defined SYS_TARGET_STM32F411CEU6) || (defined EMULATOR_STM32F411CEU6)
  WRITE_REG(RCC->PLLCFGR, RCC_PLLSOURCE_HSE | (25<<RCC_PLLCFGR_PLLM_Pos) | (192<<RCC_PLLCFGR_PLLN_Pos) | (((RCC_PLLP_DIV2>>1)-1)<<RCC_PLLCFGR_PLLP_Pos) | (4<<RCC_PLLCFGR_PLLQ_Pos));
#elif (defined SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F405RGT6)
  WRITE_REG(RCC->PLLCFGR, RCC_PLLSOURCE_HSE | (16<<RCC_PLLCFGR_PLLM_Pos) | (336<<RCC_PLLCFGR_PLLN_Pos) | (((RCC_PLLP_DIV2>>1)-1)<<RCC_PLLCFGR_PLLP_Pos) | (7<<RCC_PLLCFGR_PLLQ_Pos));
#else
  #error "Unimplemented"
#endif

  SET_BIT( RCC->CR, RCC_CR_PLLON ); /*!< Bit-Banding Feature: equivalent to `RCC_CR_PLLON_BB = 1;` */

#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6)
  /* Wait till PLL is enabled (0=unlocked; 1=locked) */
  while(0==READ_BIT(RCC->CR, RCC_CR_PLLRDY));
#endif
  
  if(FLASH_LATENCY_3 > READ_BIT((FLASH->ACR), FLASH_ACR_LATENCY)){
    ((*(volatile u8 *)0x40023C00U) = (u8)(FLASH_ACR_LATENCY));
    if(FLASH_LATENCY_3 != READ_BIT((FLASH->ACR), FLASH_ACR_LATENCY)){
      // Error
    }
  }
  

  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_HCLK_DIV16);
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, (RCC_HCLK_DIV16 << 3));
  MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_SYSCLK_DIV1);

  /* System Clock Source: PLLCLK */
  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_SYSCLKSOURCE_PLLCLK);

#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6)
  /* Wait until the clock is stable */
  while( (RCC_SYSCLKSOURCE_PLLCLK<<RCC_CFGR_SWS_Pos) != READ_BIT( RCC->CFGR, RCC_CFGR_SWS) );
#endif

  if(FLASH_LATENCY_3 < READ_BIT((FLASH->ACR), FLASH_ACR_LATENCY)){
    ((*(volatile u8 *)0x40023C00U) = (u8)(FLASH_ACR_LATENCY));
    if(FLASH_LATENCY_3 != READ_BIT((FLASH->ACR), FLASH_ACR_LATENCY)){
      // Error
    }
  }

  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_HCLK_DIV2);
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_HCLK_DIV1);

  uwTickFreq = HAL_TICK_FREQ_1KHZ;
#if (defined SYS_TARGET_STM32F411CEU6) || (defined EMULATOR_STM32F411CEU6)
  SystemCoreClock = 96000000;
#elif (defined SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F405RGT6)
  SystemCoreClock = 84000000;  
#else
  #error "Unimplemented"
#endif

SysTick_Config(SystemCoreClock/(1000/uwTickFreq));

  if (uwTickPrio < (1UL << __NVIC_PRIO_BITS)){
    HAL_NVIC_SetPriority(SysTick_IRQn, uwTickPrio, 0U);
  }
  
  return;
}


/**
 * @brief Get the Chip Unique ID
 * @param [out] len - Length of uuid (A constant number during the compiling time)
 * @return Return a numeric array containing chip uuid  
 */
const u8* bsp_cpu_chip_uuid( size_t *len){
  *len = 12;
  return (const u8*)UID_BASE;
}


#ifdef __cplusplus
}
#endif
