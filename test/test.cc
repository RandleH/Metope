



#include <stdio.h>
#include <unistd.h>
#include "test.hh"


#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6)
  #include "stm32f4xx_hal.h"
  #include "stm32f4xx_hal_uart.h"
#endif



/* ************************************************************************** */
/*                      Retargeting Standard I/O Stream                       */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"
{
#endif



extern USART_HandleTypeDef husart2;

void uart_init( void){
  /* Disable I/O buffering for STDOUT stream, so that
   * chars are sent out as soon as they are printed. */
  setvbuf(stdout, NULL, _IONBF, 0);
}

int _isatty(int fd);

int _write(int fd, char* ptr, int len){
  HAL_StatusTypeDef hstatus;

  if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
    hstatus = HAL_USART_Transmit( &husart2, (uint8_t *) ptr, len, HAL_MAX_DELAY);
    if (hstatus == HAL_OK)
      return len;
    else
      return EIO;
  }
  errno = EBADF;
  return -1;
}

int _close(int fd);
int _lseek(int fd, int ptr, int dir);
int _read(int fd, char* ptr, int len);
int _fstat(int fd, struct stat* st);


#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE{
  char tOut = char(ch);

  return _write(STDOUT_FILENO, &tOut, 1);
}

#ifdef __cplusplus
} //extern "C"
#endif



/* ************************************************************************** */
/*                         Test Bench Abstract Level                          */
/* ************************************************************************** */
std::ostream& operator<<(std::ostream& out, const TestBone& testunit_client){
  return out;
}
