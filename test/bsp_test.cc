


#include <bitset>
#include "test.hh"

#include "cmn_test.hh"
#include "cmn_type.h"
#include "cmn_math.h"




extern HumanInteractionTest tb_infra_hmi;



class TestBspUsartEcho : public TestUnitWrapper_withInputOutput<std::array<char,6>,char>{
public:
  // TestBspUsartEcho():TestUnitWrapper("test_bsp_usart_echo"){}
  TestBspUsartEcho():TestUnitWrapper_withInputOutput("test_bsp_usart_echo", std::cin, std::cout){}

  bool run( std::array<char,6>& input, char& ref ){
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
  ;

}
