

#include "device.h"


#ifndef CMN_CALLBACK_H
#define  CMN_CALLBACK_H



#ifdef __cplusplus
extern "C"{
#endif

void cmn_callback_screen_spi_completed(SPI_HandleTypeDef *hspi);
void cmn_callback_user_key_detected(uint16_t key);

#ifdef __cplusplus
}
#endif






#endif
