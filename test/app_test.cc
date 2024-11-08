


#include "global.h"
#include "app_test.hh"
#include "app_gui.h"
#include "app_lvgl.h"
#include "test.hh"
#include "cmn_type.h"
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
    tGuiClock1 client = {
      .pScreen    = NULL,
      .pPinHour   = NULL,
      .pPinMinute = NULL,
      .time       = {0}
    };
    client.time.year     = 0;
    client.time.month    = 4;
    client.time.day      = 1;
    client.time.hour     = 13;
    client.time.minute   = 42;
    client.time.second   = 32;
    
    ui_Clock1_screen_init( &client);

    bool          result   = false;
    cmnDateTime_t datetime = {0};

    while(1){
      if(!get_user_input(result, datetime)){
        ui_Clock1_set_time( &client, datetime);
        lv_disp_load_scr( client.pScreen);
        app_lvgl_flush_all();
      }else{
        break;
      }
    }
    
    return result;
  }
};



void add_app_test( void){
  tb_infra_hmi.insert(
    TestAppGuiMainDisplayClock1(),
    std::array<cmnDateTime_t,4>{{
      {
        
      },{0},{0},{0}
    }},
    '\0'
  );
}
