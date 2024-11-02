



#if (defined SYS_TARGET_STM32F411CEU6) || defined (SYS_TARGET_STM32F405RGT6)
  #include "stm32f4xx_hal.h"
#endif




#ifdef __cplusplus
extern "C"{
#endif


/**
 * @brief
 * @addtogroup MachineDependent
 */
void bsp_cpu_clock_init( void){
  /* Set the new HSE configuration */
  SET_BIT(RCC->CR, RCC_CR_HSEON);

  /* Wait till HSE is ready */
  while(0==READ_BIT(RCC->CR, RCC_CR_HSERDY)); 

  /* Make sure HSI is NOT used as system clock or PLL source */
  // Skipped the checker for memory saving

  MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, RCC_HSICALIBRATION_DEFAULT<<RCC_CR_HSITRIM_Pos);

  /* Make sure PLL is NOT used as system clock*/
  // Skipped the checker for memory saving
  
  /* Disable the main PLL. */
  CLEAR_BIT( RCC->CR, RCC_CR_PLLON ); /*!< Bit-Banding Feature: equivalent to `RCC_CR_PLLON_BB = 0;` */
  
  /* Wait till PLL is disabled (0=unlocked; 1=locked) */
  while(1==READ_BIT(RCC->CR, RCC_CR_PLLRDY)); 
  
  WRITE_REG(RCC->PLLCFGR, RCC_PLLSOURCE_HSE | (25<<RCC_PLLCFGR_PLLM_Pos) | (192<<RCC_PLLCFGR_PLLN_Pos) | (((RCC_PLLP_DIV2>>1)-1)<<RCC_PLLCFGR_PLLP_Pos) | (4<<RCC_PLLCFGR_PLLQ_Pos));

  SET_BIT( RCC->CR, RCC_CR_PLLON ); /*!< Bit-Banding Feature: equivalent to `RCC_CR_PLLON_BB = 1;` */

  /* Wait till PLL is enabled (0=unlocked; 1=locked) */
  while(0==READ_BIT(RCC->CR, RCC_CR_PLLRDY));

  return;
}




#ifdef __cplusplus
}
#endif
