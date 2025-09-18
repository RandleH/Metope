


/* ************************************************************************** */
/*                                  Includes                                  */
/* ************************************************************************** */
#include "device.h"
#include "global.h"
#include "trace.h"
#include "FreeRTOS.h"
#include "event_groups.h"


#ifdef __cplusplus
extern "C"{
#endif


#define SPI_DEFAULT_TIMEOUT 100U
#define SPI_BSY_FLAG_WORKAROUND_TIMEOUT 1000U /*!< Timeout 1000 µs             */


/**
  * @brief  Handle SPI Communication Timeout.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *              the configuration information for SPI module.
  * @param  Flag SPI flag to check
  * @param  State flag state to check
  * @param  Timeout Timeout duration
  * @param  Tickstart tick start value
  * @retval HAL status
  */
// static HAL_StatusTypeDef SPI_WaitFlagStateUntilTimeout(SPI_HandleTypeDef *hspi, uint32_t Flag, FlagStatus State,
                                                      //  uint32_t Timeout, uint32_t Tickstart);


/**
  * @brief  Handle the check of the RXTX or TX transaction complete.
  * @param  hspi SPI handle
  * @param  Timeout Timeout duration
  * @param  Tickstart tick start value
  * @retval HAL status
  */
// static HAL_StatusTypeDef SPI_EndRxTxTransaction(SPI_HandleTypeDef *hspi, uint32_t Timeout, uint32_t Tickstart);

/**
  * @brief  DMA SPI transmit process complete callback.
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */


/**
 * @addtogroup MachineDependent
 */
#if 1
void cmn_callback_screen_spi_completed(SPI_HandleTypeDef *hspi)
#else
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
#endif
{
#if 0
  assert( ((SPI2->CR & DMA_SxCR_CIRC) != DMA_SxCR_CIRC) );
#endif
  /* Disable ERR interrupt */
  CLEAR_BIT( SPI2->CR2, SPI_IT_ERR);
  /* Disable Tx DMA Request */
  CLEAR_BIT( SPI2->CR2, SPI_CR2_TXDMAEN);
  
  /* Check the end of the transaction */
  while(!READ_BIT( SPI2->SR, SPI_SR_TXE));
  while(READ_BIT( SPI2->SR, SPI_SR_BSY));
  
  /* Clear overrun flag in 2 Lines communication mode because received data is not read */
#if 0
  __HAL_SPI_CLEAR_OVRFLAG(hspi);
#else
  __IO uint32_t tmpreg_ovr = 0x00U;
  tmpreg_ovr = SPI2->DR;
  tmpreg_ovr = SPI2->SR;
  UNUSED(tmpreg_ovr);
#endif
  // SPI_DMATransmitCplt(&hdma_spi2_tx);
}


void cmn_callback_user_key_detected(uint16_t key) {
#if 1
  if( READ_BIT(EXTI->PR, key) ){
    EXTI->PR = key;
  }
#else
  HAL_GPIO_EXTI_IRQHandler(key);
#endif

  cmnEventBitmap_t cmn_event_bitmap = 0;
  if (key == KEY_M_Pin) {
    cmn_event_bitmap = CMN_EVENT_USER_KEY_M | CMN_EVENT_UPDATE_RTC;
  }else if (key == KEY_L_Pin) {
    cmn_event_bitmap = CMN_EVENT_USER_KEY_L;
  }else if (key == KEY_R_Pin) {
    cmn_event_bitmap = CMN_EVENT_USER_KEY_R;
  }

  TRACE_DEBUG("CMN - Received user key pressed: %d", key);

  tRtos *p_rtos = &metope.rtos;
  if(p_rtos->status.running){
    BaseType_t xHigherPriorityTaskWoken, xResult;
    xHigherPriorityTaskWoken = pdFALSE;
    xResult = xEventGroupSetBitsFromISR( p_rtos->event._handle, cmn_event_bitmap, &xHigherPriorityTaskWoken );
    if( xResult != pdFAIL ){
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
  }
}



#ifdef __cplusplus
}
#endif


