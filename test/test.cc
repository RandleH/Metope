/**
 ******************************************************************************
 * @file    test.cc
 * @author  RandleH
 * @brief   General Test Program Model
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
#include <stdio.h>
#include <unistd.h>
#include "test.hh"
#if (defined SYS_TARGET_STM32F411CEU6) || (defined SYS_TARGET_STM32F405RGT6) || (defined EMULATOR_STM32F411CEU6) || (defined EMULATOR_STM32F405RGT6)
  #include "stm32f4xx_hal.h"
  #include "stm32f4xx_hal_uart.h"
#endif



#define STATIC_SWAP( a, b)   do{ (a)^=(b); (b)^=(a); (a)^=(b); }while(0)



#ifdef __cplusplus
extern "C"{
#endif

int _read(int fd, char* ptr, int len);
int _write(int fd, const char* ptr, int len);

#ifdef __cplusplus
}
#endif


/* ************************************************************************** */
/*                           Customized I/O stream                            */
/* ************************************************************************** */
#if USE_LOCAL_IOSTREAM
namespace local{

local::istream cin  = {};
local::ostream cout = {};
const char* endl = "\n";


bool istream::operator>>(std::string& str){
  std::stringstream s;
  char x = '\0';

  do{
    if(1==_read(STDIN_FILENO, &x, 1)){
      s << x;
    }else{
      return false;
    }
  }while(x!='\r');
  str = s.str();
  return true;
}

bool istream::operator>>(char& c){
  if(1==_read(STDIN_FILENO, &c, 1)){
    return true;
  }
  return false;
}


ostream& ostream::operator<<(uint32_t x){
  char    buf[11] = {0};
  uint8_t cnt     = 0;
  ldiv_t a;
  do{
    a = ldiv(x,10);
    buf[cnt++] = a.rem+'0';
    x = a.quot;
  }while(x);

  size_t nel = 0;
  --cnt;
  while(cnt>nel){
    /* Reuse `x` variable for swapping */
    x = buf[cnt];
    buf[cnt] = buf[nel];
    buf[nel] = x;
    --cnt; ++nel;
  }
  _write( STDOUT_FILENO, buf, cnt);
  return (*this);
}

ostream& ostream::operator<<(unsigned int x){
  return (*this)<<((uint32_t)x);
}

/**
 * @todo: Add support for signed values.
 */
ostream& ostream::operator<<( int32_t x){
  return (*this)<<((uint32_t)x);
}

/**
 * @todo: Add support for signed values.
 */
ostream& ostream::operator<<( int x){
  return (*this)<<((uint32_t)x);
}


ostream& ostream::operator<<(char c){
  _write( STDOUT_FILENO, &c, 1);
  return (*this);
}

ostream& ostream::operator<<(const char *str){
  _write( STDOUT_FILENO, str, strlen(str));
  return (*this);
}

}
#endif



/* ************************************************************************** */
/*                       Standard I/O Stream Retargeting                      */
/* ************************************************************************** */
#ifdef __cplusplus
extern "C"
{
#endif


extern UART_HandleTypeDef huart2;

/**
 * @brief
 * @addtogroup MachineDependent
 */
void uart_init( void){
  /* Disable I/O buffering for STDOUT stream, so that
   * chars are sent out as soon as they are printed. */
  setvbuf(stdout, NULL, _IONBF, 0);
}

int _isatty(int fd);

/**
 * @addtogroup MachineDependent
 */
int _write(int fd, const char* ptr, int len){
  HAL_StatusTypeDef hstatus;

  if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
    hstatus = HAL_UART_Transmit( &huart2, (uint8_t *) ptr, len, HAL_MAX_DELAY);
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

/**
 * @addtogroup MachineDependent
 */
int _read(int fd, char* ptr, int len){
  HAL_StatusTypeDef hstatus;

  if (fd == STDIN_FILENO) {
    hstatus = HAL_UART_Receive( &huart2, (uint8_t *) ptr, 1, HAL_MAX_DELAY);
    if (hstatus == HAL_OK)
      return 1;
    else
      return EIO;
  }
  errno = EBADF;
  return -1;
}

int _fstat(int fd, struct stat* st);


#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
  #define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  #define GETCHAR_PROTOTYPE int fgetc(void);
#endif /* __GNUC__ */



PUTCHAR_PROTOTYPE{
  char tOut = char(ch);

  return _write(STDOUT_FILENO, &tOut, 1);
}

GETCHAR_PROTOTYPE{
  char tIn = '\0';
  _read(STDIN_FILENO, &tIn, 1);
  return tIn;
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
