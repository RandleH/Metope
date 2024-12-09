/**
 ******************************************************************************
 * @file    bsp_test.cc
 * @author  RandleH
 * @brief   BSP Test Program
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
#include <stdlib.h>
#include <bitset>
#include <regex>
#include "test.hh"
#include "global.h"

#include "cmn_test.hh"
#include "cmn_type.h"
#include "cmn_math.h"
#include "cmn_utility.h"

#include "bsp_screen.h"
#include "bsp_rtc.h"



/* ************************************************************************** */
/*                          Class: TestBspUsartEcho                           */
/* ************************************************************************** */

namespace paramsTestBspUsartEcho{
typedef std::array<char,6> Input;
typedef char               Output;  // Dummy
}
class TestBspUsartEcho : public TestUnitWrapper_withInputOutput
<paramsTestBspUsartEcho::Input, paramsTestBspUsartEcho::Output>{
public:
  TestBspUsartEcho():TestUnitWrapper_withInputOutput("test_bsp_usart_echo"){}
  
  bool run( std::array<char,6>& input, char& ref ) override{
    for( const auto& ref_ch : input){
      char       dut_ch = '\0';
      
      this->_cout<<"\nPlease enter this letter: "<<CONSOLE_FMT_BOLD<< CONSOLE_FMT_BLUE<<ref_ch<<CONSOLE_FMT_RESET<<endl;
      this->_cin>>dut_ch;
      /**
       * @todo: Considering if user can not type anything by adding some timeout checker.
       */
      if(ref_ch!=dut_ch){
        this->_err_msg<<"Echo Mismatched. dut_ch="<<(int)(dut_ch)<<'('<<dut_ch<<')'<<" ref_ch="<<(int)ref_ch<<'('<<ref_ch<<')'<<endl;
        return false;
      }
    }
    return true;
  }
};



/* ************************************************************************** */
/*                       Class: TestBspScreenBrightness                       */
/* ************************************************************************** */
namespace paramsTestBspScreenBrightness{
typedef char Input;   // Dummy
typedef char output;  // Dummy
}
class TestBspScreenBrightness : public TestUnitWrapper_withInputOutput<char,char>{
public:
  TestBspScreenBrightness():TestUnitWrapper_withInputOutput("test_bsp_screen_brightness"){}
  bool run( char& input, char& ref ) override{
    std::string s;
    bool result = true;

    this->_cout<<"\nScreen Brightness Test - Please enter a percentage value:"<<endl;
    this->_cout<<"Press [Q] to quit; Press [E] to reject;"<<endl;
    bsp_screen_on();
    while (this->_cin >> s) {
      cmnBoolean_t isValid  = false;
      float      brightness = cmn_utility_str2float(s.c_str(), &isValid);
      if(isValid){
        if(brightness<100.0 && brightness>=0.0){
          bsp_screen_set_bright( lround(brightness*2048)/100 );
        }else{
          this->_cout<<"Please enter a value between 0~100:"<<endl;
        }
      }else{
        if(s.length()==1){
          if(s[0]=='Q' || s[0]=='q'){
            result = true;
            break;
          }else if (s[0]=='E' || s[0]=='e'){
            result = false;
            this->_err_msg<<"User objection."<<endl;
            break;
          }
        }
        this->_cout<<"Press [Q] to quit; Press [E] to reject;"<<endl;
        this->_cout<<"Please enter a valid number:"<<endl;
      }
    }
    bsp_screen_off();
    return result;
  }
};




/* ************************************************************************** */
/*                       Class: TestBspScreenSmoothness                       */
/* ************************************************************************** */
class TestBspScreenSmoothness : public TestUnitWrapper_withInputOutput<char,char>{
public:
  TestBspScreenSmoothness():TestUnitWrapper_withInputOutput("test_bsp_screen_smoothness"){}

  bool run( char& input, char& ref ) override{
    std::string s;
    bool result = true;
    bool flag = false;
    this->_cout<<"\nScreen Smoothness Test - Press any other keys to continue."<<endl;
    this->_cout<<"Press [Q] to quit; Press [E] to reject;"<<endl;

    bsp_screen_off();
    
    while (this->_cin >> s) {
      if(s.length()==1){
        if(s[0]=='Q' || s[0]=='q'){
          result = true;
          break;
        }else if (s[0]=='E' || s[0]=='e'){
          result = false;
          this->_err_msg<<"User objection."<<endl;
          break;
        }
      }
      if(!flag){
        bsp_screen_smooth_on();
      }else{
        bsp_screen_smooth_off();
      }
      flag = !flag;
    }

    bsp_screen_off();
    return result;
  }
};



/* ************************************************************************** */
/*                          Class: TestBspScreenFill                          */
/* ************************************************************************** */
class TestBspScreenFill : public TestUnitWrapper_withInputOutput<char,char>{
public:
  TestBspScreenFill():TestUnitWrapper_withInputOutput("test_bsp_screen_filling"){}

  bool run( char& input, char& ref ) override{
    std::string s;
    bool result = true;
    
    bsp_screen_on();
    bsp_screen_set_bright(BSP_SCREEN_DEFAULT_BRIGHTNESS);

    this->_cout<<"\nScreen Fill Test - "<<"Please enter the hex color:"<<endl;
    this->_cout<<"Example: 0x00FF0000 (Red) | 0x0000FF00 (Green)"<<endl;
    this->_cout<<"\nPress [Q] to quit; Press [E] to reject;"<<endl;
    while (this->_cin >> s) {
      cmnBoolean_t isValid  = false;
      int32_t color = cmn_utility_str2hex(s.c_str(), &isValid);
      
      if(s[0]=='Q' || s[0]=='q'){
        result = true;
        break;
      }else if (s[0]=='E' || s[0]=='e'){
        result = false;
        this->_err_msg<<"User objection."<<endl;
        break;
      }else if(isValid){
        if( color>0x00FFFFFF){
          this->_cout<<"Hex Color is [0:0xFFFFFF] with `0x00RRGGBB` format. Please enter a valid value:"<<endl;
        }else{
          u16 r = ((((color&0xFF0000)<<5>>8)>>16)<<11);
          u16 g = ((((color&0x00FF00)<<6>>8)>>8)<<5);
          u16 b = ((((color&0x0000FF)<<5>>8)>>0)<<0);

          bsp_screen_fill( (r|g|b), 0, 0, BSP_SCREEN_WIDTH-1, BSP_SCREEN_HEIGHT-1);
        }
      }else{
        this->_cout<<"Hex Color is [0:0xFFFFFF] with `0x00RRGGBB` format. Please enter a valid value:"<<endl;
      }
    }
    return result;
  }
};


/* ************************************************************************** */
/*                        Class: TestBspScreenDrawArea                        */
/* ************************************************************************** */
class TestBspScreenDrawArea : public TestUnitWrapper_withInputOutput<char,char>{
public:
  TestBspScreenDrawArea():TestUnitWrapper_withInputOutput("test_bsp_screen_draw_area"){}
  bool run( char& input, char& ref ) override{
    std::string s;
    bool result = true;
    
    bsp_screen_on();
    bsp_screen_set_bright(BSP_SCREEN_DEFAULT_BRIGHTNESS);

    this->_cout<<"\nScreen Draw Area Test - Please enter the area:"<<endl;
    this->_cout<<"\nPress [Q] to quit; Press [E] to reject;"<<endl;

    u32 cood[4] = {0};
    const char* msg[] = {"[xs]","[ys]","[xe]","[ye]"};
    cmnBoolean_t isValid = false;
    
    for( u8 i=0; i<4; ++i){
      this->_cout<<"Please enter the coodinate "<<msg[i]<<endl;
      while (this->_cin >> s){
        cood[i] = cmn_utility_str2dec(s.c_str(), &isValid);
        if(!isValid){
          this->_cout<<"Please enter a number!"<<endl;
        }else if(cood[i]<0 || cood[i]>= ((i%2==0)?BSP_SCREEN_WIDTH:BSP_SCREEN_HEIGHT) ){
          this->_cout<<"Out of region!"<<endl;
        }else if(i>=3 && (cood[3]<cood[1] || cood[2]<cood[0])){
          this->_cout<<msg[2]<<" and "<<msg[3]<<" must be greater than the start coods!"<<endl;
        }else{
          break;
        }
      }
    }
    
    this->_cout<<"\nPlease enter the hex color:"<<endl;
    this->_cout<<"Example: 0x00FF0000 (Red) | 0x0000FF00 (Green)"<<endl;
    while (this->_cin >> s) {
      int32_t color = cmn_utility_str2hex(s.c_str(), &isValid);
      
      if(s[0]=='Q' || s[0]=='q'){
        result = true;
        break;
      }else if (s[0]=='E' || s[0]=='e'){
        result = false;
        this->_err_msg<<"User objection."<<endl;
        break;
      }else if(isValid){
        if( color>0x00FFFFFF){
          this->_cout<<"Hex Color is [0:0xFFFFFF] with `0x00RRGGBB` format. Please enter a valid value:"<<endl;
        }else{
          u16 r = ((((color&0xFF0000)<<5>>8)>>16)<<11);
          u16 g = ((((color&0x00FF00)<<6>>8)>>8)<<5);
          u16 b = ((((color&0x0000FF)<<5>>8)>>0)<<0);

          bsp_screen_fill( (r|g|b), cood[0], cood[1], cood[2], cood[3]);
        }
      }else{
        this->_cout<<"Hex Color is [0:0xFFFFFF] with `0x00RRGGBB` format. Please enter a valid value:"<<endl;
      }
    }
    return result;
  }
};




/* ************************************************************************** */
/*                             Class: TestBspRTC                              */
/* ************************************************************************** */
namespace paramsTestBspRTC{
typedef char Input;   // Dummy
typedef char Output;  // Dummy
}
class TestBspRTC : public TestUnitWrapper_withInputOutput<paramsTestBspRTC::Input, paramsTestBspRTC::Output>{
private:
  void usage(void){
    this->_cout<<"\nPress [T] to get current time; Press [Q] to quit; Press [E] to reject;"<<endl;
    this->_cout<<"Type time in format `YYYY-MM-DD-HH:MM:SS` to set time."<<endl;
  }

public:
  TestBspRTC():TestUnitWrapper_withInputOutput("test_bsp_rtc"){}
  bool run( paramsTestBspRTC::Input& input, paramsTestBspRTC::Output& ref ) override{
    std::string s;
    cmnBoolean_t result = false;

    this->_cout<<"\n>>>>>> RTC Module Test <<<<<<"<<endl;
    usage();

#if 0
    cmnDateTime_t time = cmn_utility_set_time_from_iso(__TIMESTAMP__);
    bsp_rtc_set_time(time);
#else
    cmnDateTime_t time;
#endif
    while (this->_cin >> std::skipws >> s) {
      if(s.length()==1){
        if(s[0]=='Q' || s[0]=='q'){
          result = true;
          break;
        }else if (s[0]=='E' || s[0]=='e'){
          result = false;
          this->_err_msg<<"User objection."<<endl;
          break;
        }else if (s[0]=='T' || s[0]=='t'){
          u8 raw_7[7];
          time = bsp_rtc_get_time__debug(raw_7);
          this->_cout<<"Current Time: "<<time.year+2024<<'/'<<time.month<<'/'<<time.day<<' '<<time.hour<<'-'<<time.minute<<'-'<<time.second<<endl;

          for(int i=0; i<7; ++i){
            this->_cout << '[' << i << ']'<<'='<<"0x"<<std::hex<<((int)(raw_7[i]))<<endl;
          }
          this->_cout<< std::dec << endl;

        }
      }else{
        std::regex e("(\\d+)-(\\d+)-(\\d+)-(\\d+):(\\d+):(\\d+)");
        std::smatch m;
        if(!std::regex_search (s,m,e)){
          this->_cout<<"Can NOT parse the timming inputs."<<endl;
          continue;
        }
        time.year   = atoi(m[1].str().c_str())-2024;
        time.month  = atoi(m[2].str().c_str());
        time.day    = atoi(m[3].str().c_str());
        time.hour   = atoi(m[4].str().c_str());
        time.minute = atoi(m[5].str().c_str());
        time.second = atoi(m[6].str().c_str());
        bsp_rtc_set_time(time);
      }
      usage();
    }

    return result;
  }
};



/**
 * @brief
 * @note  `std::array<bspScreenCood_t,4>` is the area coodinates set.
 * @note  Total num rounds: 3
 */
class TestBspScreenDrawAreaStatic : public TestUnitWrapper_withInputOutput<std::array<std::array<bspScreenCood_t,4>,3>,char>{
public:
  TestBspScreenDrawAreaStatic():TestUnitWrapper_withInputOutput("test_bsp_screen_draw_area_static"){}
  bool run( std::array<std::array<bspScreenCood_t,4>,3>& input, char& ref ) override{
    bool result = false;
    std::string s;

    bsp_screen_fill( 0x688, 0,0, BSP_SCREEN_WIDTH-1, BSP_SCREEN_HEIGHT-1);
    for( const auto &item : input){
      size_t len = (1+item[2]-item[0])*(1+item[3]-item[1])*sizeof(bspScreenPixel_t);
      bspScreenPixel_t *buf = (bspScreenPixel_t*)malloc(len);
      if(buf==NULL){
        this->_err_msg<<"Can NOT allocate enough memory"<<endl;
        result = false;
      }else{
        memset( buf, 0xFF, len);
        bsp_screen_refresh( buf, item[0], item[1], item[2], item[3]);
        free(buf);
      }
    }

    this->_cout<<"\nPress [Q] to quit; Press [E] to reject;"<<endl;

    while (this->_cin >> s) {
      if(s[0]=='Q' || s[0]=='q'){
        result = true;
        break;
      }else if (s[0]=='E' || s[0]=='e'){
        result = false;
        this->_err_msg<<"User objection."<<endl;
        break;
      }else{
        this->_cout<<"\nPress [Q] to quit; Press [E] to reject;"<<endl;
      }
    }
    return result;
  }
};


/* ************************************************************************** */
/*                          Class: TestBspGyroscope                           */
/* ************************************************************************** */
#include "bsp_gyro.h"
namespace paramsTestBspGyroscope{
typedef char Input;   // Dummy
typedef char Output;  // Dummy
}

class TestBspGyroscope : public TestUnitWrapper_withInputOutput<paramsTestBspGyroscope::Input,paramsTestBspGyroscope::Output>{
public:
  TestBspGyroscope():TestUnitWrapper_withInputOutput("test_bsp_gyroscope"){}

  bool run( paramsTestBspGyroscope::Input &i,paramsTestBspGyroscope::Output &o ) override{
    string s;
    bool result = false;
    
    int chip_id = bsp_gyro_get_chip_id();
    this->_cout << "Chip ID="<<chip_id << endl;
    while(this->_cin >> s){
      if(s.length()==1){
        if(s[0]=='Q' || s[0]=='q'){
          result = true;
          break;
        }else if (s[0]=='E' || s[0]=='e'){
          result = false;
          this->_err_msg<<"User objection."<<endl;
          break;
        }else{
        }
      }
    }
    bsp_gyro_switch(OFF);
    return result;
  }
};


/**
 * @addtogroup TestBench
 */
void add_bsp_test(void){
#if (defined INCLUDE_TB_BSP) && (INCLUDE_TB_BSP==1)
  tb_infra_bsp
    .insert(
      TestBspUsartEcho(),
      std::array<char,6>{{'R','a','n','d','l','e'}},
      '\0'
    )
    // .insert(
    //   TestBspScreenBrightness(),
    //   (char)'\0',
    //   (char)'\0'
    // )
    // .insert(
    //   TestBspScreenSmoothness(),
    //   (char)'\0',
    //   (char)'\0'
    // )
    // .insert(
    //   TestBspScreenFill(),
    //   (char)'\0',
    //   (char)'\0'
    // )
    // .insert(
    //   TestBspScreenDrawArea(),
    //   (char)'\0',
    //   (char)'\0'
    // )
    // .insert(
    //   TestBspScreenDrawAreaStatic(),
    //   std::array<std::array<bspScreenCood_t,4>,3>{{
    //     {66,66,77,77},
    //     {120,120,140,150},
    //     {190,100, 195,130}
    //   }},
    //   (char)'\0'
    // )
    .insert(
      TestBspRTC(),
      '\0',
      '\0'
    )
    .insert(
      TestBspGyroscope(),
      '\0',
      '\0'
    )
  ;
#endif

}
