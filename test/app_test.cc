/**
 ******************************************************************************
 * @file    app_test.cc
 * @author  RandleH
 * @brief   Application Test Program
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
#include "global.h"
#include "app_test.hh"
#include "app_rtos.h"
#include "app_clock.h"
#include "app_lvgl.h"
#include "test.hh"
#include "cmn_type.h"
#include "cmn_delay.h"
#include "bsp_screen.h"
#include "lvgl.h"


/* ************************************************************************** */
/*                             Default Namespace                              */
/* ************************************************************************** */


class TestAppGuiMainDisplayClock1 : public TestUnitWrapper_withInputOutput<std::array<cmnDateTime_t,4>,u8>{
private:
  bool get_user_input(bool &verdict_result, cmnDateTime_t &date){
    std::string s;
    this->_cout<<"Please enter a date time: YYYYMMDDHHMMSS"<<endl;
    this->_cout<<"\nPress [Q] to quit; Press [E] to reject;"<<endl;
    while (this->_cin >> s) {
      if(s[0]=='Q' || s[0]=='q'){
        verdict_result = true;
        return true;
      }else if (s[0]=='E' || s[0]=='e'){
        verdict_result = false;
        this->_err_msg<<"User objection."<<endl;
        return true;
      }else if( (4+2+2+2+2+2)!=s.length()){
        this->_cout<<"Please enter a valid format: YYYYMMDDHHMMSS"<<endl;
      }else{
        date.year   = 2024-(1000*(s[0]-'0') + 100*(s[1]-'0') + 10*(s[2]-'0') + (s[3]-'0'));
        date.month  = 10*(s[4]-'0')+(s[5]-'0');
        date.day    = 10*(s[6]-'0')+(s[7]-'0');
        date.hour   = 10*(s[8]-'0')+(s[9]-'0');
        date.minute = 10*(s[10]-'0')+(s[11]-'0');
        date.second = 10*(s[12]-'0')+(s[13]-'0');
        break;
      }
    }
    return false;
  }

public:
  TestAppGuiMainDisplayClock1():TestUnitWrapper_withInputOutput("test_app_main_disp_clock1"){}

  bool run( std::array<cmnDateTime_t,4>& input, u8& ref) override{
    tAppGuiClockParam client = {
      .pScreen    = NULL,
      .pPinHour   = NULL,
      .pPinMinute = NULL,
    };
    
    app_clock_gui_switch(kAppGuiClock_ClockModern);

    bool          result   = false;
    cmnDateTime_t datetime = {0};

    while(1){
      if(!get_user_input(result, datetime)){

        metope.app.clock.gui.set_time( &metope.app.clock.gui.param, datetime);

        lv_disp_load_scr( client.pScreen);
        app_lvgl_flush_all();
      }else{
        break;
      }
    }
    
    return result;
  }
};


namespace paramsTestAppTaskFreeRTOS{

/**
 * @note: 3 Task functions with their customized stack depth
 */
typedef std::array<std::pair<TaskFunction_t,u16>,4> Input;

/**
 * @note: No output
 */
typedef u8 Output;


} /* Namespace paramsTestAppTaskFreeRTOS */

class TestAppTaskFreeRTOS : public TestUnitWrapper_withInputOutput< paramsTestAppTaskFreeRTOS::Input ,paramsTestAppTaskFreeRTOS::Output>{
private:

  TaskHandle_t _handle[3];

public:
  
  static void task2(void*param){
    while(1){
      cmnBoolean_t async_mode = true;
      cmn_tim2_sleep(1000, async_mode);
      bsp_led_toggle();
    }
  }
  TestAppTaskFreeRTOS():TestUnitWrapper_withInputOutput("test_app_task_freertos"){}

  bool run( paramsTestAppTaskFreeRTOS::Input& input, paramsTestAppTaskFreeRTOS::Output& ref) override{
    cmnBoolean_t result;
    cmnBoolean_t ret;

    for(unsigned int i=0; i<input.size(); ++i){
      ret = xTaskCreate(\
        input[i].first,\
        "0",\
        input[i].second,\
        NULL,\
        kAppPriority_IMPORTANT,\
        &_handle[i]
      );

      if(ret!=pdPASS){
        this->_err_msg<<"Failed to create Task 0"<<endl;
        result = false;
        break;
      }
    }
    
    app_rtos_start();
    
    // if(result==false){
    //   return result;
    // }else{
    // }

    while(1);

    return result;
  }
};


void add_app_test( void){


#if (defined INCLUDE_TB_BSP) && (INCLUDE_TB_BSP==1)
  // tb_infra_bsp.insert(
  //   TestAppGuiMainDisplayClock1(),
  //   std::array<cmnDateTime_t,4>{{
  //     {
        
  //     },{0},{0},{0}
  //   }},
  //   '\0'
  // );
#endif

  tb_infra_os.insert(
    TestAppTaskFreeRTOS(),
    paramsTestAppTaskFreeRTOS::Input{{
      {app_clock_main, 512},
      {bsp_screen_main, 512},
      {TestAppTaskFreeRTOS::task2, 128},
      {bsp_screen_onoff, 128}
    }},
    '\0'
  );
}
