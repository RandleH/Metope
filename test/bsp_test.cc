


#include <bitset>
#include "test.hh"

#include "cmn_test.hh"
#include "cmn_type.h"
#include "cmn_math.h"
#include "cmn_utility.h"



extern HumanInteractionTest tb_infra_hmi;






class TestBspUsartEcho : public TestUnitWrapper_withInputOutput<std::array<char,6>,char>{
public:
  TestBspUsartEcho():TestUnitWrapper_withInputOutput("test_bsp_usart_echo", std::cin, std::cout){}

  bool run( std::array<char,6>& input, char& ref ) override{
    for( const auto& ref_ch : input){
      char       dut_ch = '\0';
      
      this->_cout<<"\nPlease enter this letter: "<<CONSOLE_FMT_BOLD<< CONSOLE_FMT_BLUE<<ref_ch<<CONSOLE_FMT_RESET<<std::endl;
      this->_cin>>dut_ch;
      /**
       * @todo
       *  Considering if user can not type something. Add timeout function.
       */
      if(ref_ch!=dut_ch){
        this->_err_msg<<"Echo Mismatched. dut_ch="<<(int)(dut_ch)<<'('<<dut_ch<<')'<<" ref_ch="<<(int)ref_ch<<'('<<ref_ch<<')'<<std::endl;
        return false;
      }
    }
    return true;
  }
};

#include "bsp_screen.h"
class TestBspScreenBrightness : public TestUnitWrapper_withInputOutput<char,char>{
public:
  TestBspScreenBrightness():TestUnitWrapper_withInputOutput("test_bsp_screen_brightness", std::cin, std::cout){}
  bool run( char& input, char& ref ) override{
    std::string s;
    bool result = true;

    this->_cout<<"\nScreen Brightness Test - Please enter a percentage value:"<<std::endl;
    this->_cout<<"Press [Q] to quit; Press [E] to reject;"<<std::endl;
    bsp_screen_on();
    while (std::cin >> s) {
      cmnBoolean_t isValid  = false;
      float      brightness = cmn_utility_str2float(s.c_str(), &isValid);
      if(isValid){
        if(brightness<100.0 && brightness>=0.0){
          bsp_screen_set_bright( lround(brightness*2048)/100 );
        }else{
          this->_cout<<"Please enter a value between 0~100:"<<std::endl;
        }
      }else{
        if(s.length()==1){
          if(s[0]=='Q' || s[0]=='q'){
            result = true;
            break;
          }else if (s[0]=='E' || s[0]=='e'){
            result = false;
            this->_err_msg<<"User objection."<<std::endl;
            break;
          }
        }
        this->_cout<<"Press [Q] to quit; Press [E] to reject;"<<std::endl;
        this->_cout<<"Please enter a valid number:"<<std::endl;
      }
    }
    bsp_screen_off();
    return result;
  }
};


class TestBspScreenSmoothness : public TestUnitWrapper_withInputOutput<char,char>{
public:
  TestBspScreenSmoothness():TestUnitWrapper_withInputOutput("test_bsp_screen_smoothness", std::cin, std::cout){}

  bool run( char& input, char& ref ) override{
    std::string s;
    bool result = true;
    bool flag = false;
    this->_cout<<"\nScreen Smoothness Test - Press any other keys to continue."<<std::endl;
    this->_cout<<"Press [Q] to quit; Press [E] to reject;"<<std::endl;

    bsp_screen_off();
    
    while (std::cin >> s) {
      if(s.length()==1){
        if(s[0]=='Q' || s[0]=='q'){
          result = true;
          break;
        }else if (s[0]=='E' || s[0]=='e'){
          result = false;
          this->_err_msg<<"User objection."<<std::endl;
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



class TestBspScreenFill : public TestUnitWrapper_withInputOutput<char,char>{
public:
  TestBspScreenFill():TestUnitWrapper_withInputOutput("test_bsp_screen_filling", std::cin, std::cout){}

  bool run( char& input, char& ref ) override{
    std::string s;
    bool result = true;
    
    bsp_screen_on();
    bsp_screen_set_bright(BSP_SCREEN_DEFAULT_BRIGHTNESS);

    this->_cout<<"\nScreen Fill Test - Please enter the hex color:"<<std::endl;
    this->_cout<<"Example: 0x00FF0000 (Red) | 0x0000FF00 (Green)"<<std::endl;
    this->_cout<<"\nPress [Q] to quit; Press [E] to reject;"<<std::endl;
    while (std::cin >> s) {
      cmnBoolean_t isValid  = false;
      int32_t color = cmn_utility_str2hex(s.c_str(), &isValid);
      
      if(s[0]=='Q' || s[0]=='q'){
        result = true;
        break;
      }else if (s[0]=='E' || s[0]=='e'){
        result = false;
        this->_err_msg<<"User objection."<<std::endl;
        break;
      }else if(isValid){
        if( color>0x00FFFFFF){
          this->_cout<<"Hex Color is [0:0xFFFFFF] with `0x00RRGGBB` format. Please enter a valid value:"<<std::endl;
        }else{
          u16 r = ((((color&0xFF0000)<<5>>8)>>16)<<11);
          u16 g = ((((color&0x00FF00)<<6>>8)>>8)<<5);
          u16 b = ((((color&0x0000FF)<<5>>8)>>0)<<0);

          bsp_screen_fill( (r|g|b), 0, 0, BSP_SCREEN_WIDTH-1, BSP_SCREEN_HEIGHT-1);
        }
      }else{
        this->_cout<<"Hex Color is [0:0xFFFFFF] with `0x00RRGGBB` format. Please enter a valid value:"<<std::endl;
      }
    }
    return result;
  }
};


/**
 * @addtogroup TestBench
 */
void add_bsp_test(void){
  tb_infra_hmi
    .insert(
      TestBspUsartEcho(),
      std::array<char,6>{{'R','a','n','d','l','e'}},
      '\0'
    )
    .insert(
      TestBspScreenBrightness(),
      (char)'\0',
      (char)'\0'
    )
    .insert(
      TestBspScreenSmoothness(),
      (char)'\0',
      (char)'\0'
    )
    .insert(
      TestBspScreenFill(),
      (char)'\0',
      (char)'\0'
    )
  ;

}
