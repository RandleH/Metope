

















#include "cmn_test.hh"
#include "cmn_type.h"
#include "cmn_math.h"


class TestCmnInterruptPushPopTopBasic : public TestUnitWrapper<std::vector<tCmnInterruptUnit>,std::vector<uint32_t>>{
public:
  TestCmnInterruptPushPopTopBasic():TestUnitWrapper("test_cmn_isr_mgr_basic"){}

  bool run( std::vector<tCmnInterruptUnit>& input, std::vector<uint32_t>& ref) override{
    size_t cnt = 0;
    for(auto &item : input){
      if(false==cmn_interrupt_push(&item)){
        this->_err_msg << "Failed to insert item "<<cnt<<": target_cnt="<<item.target_cnt<<" irqn="<<item.irqn<<" operation="<<item.operation<<std::endl;
        return false;
      }
      ++cnt;
    }

    if(gInterruptProcess.record_cnt!=input.size()){
      this->_err_msg << "Record length mismatched!! dut="<<gInterruptProcess.record_cnt << " ref="<<input.size()<<std::endl;
      return false;
    }

    cnt = 0;
    while(gInterruptProcess.record_cnt){
      uint32_t tmp = gInterruptProcess.record_cnt;

      uint32_t top = cmn_interrupt_top();
      if( ref[cnt]!=top){
        this->_err_msg << "Incorrect top value in the PQ. Processing cnt="<<cnt<<" dut="<<top<<" ref="<<ref[cnt]<<std::endl;
        return false;
      }

      cmn_interrupt_pop();
      if(gInterruptProcess.record_cnt != tmp-1){
        this->_err_msg << "Count wasn't decreased by 1. Processing cnt="<<cnt<<" dut="<<gInterruptProcess.record_cnt<<" ref="<<(tmp-1)<<std::endl;
        return false;
      }
    }

    return true;
  }
  
  ~TestCmnInterruptPushPopTopBasic(){
  
  };
};


class TestCmnInterruptPushPopTopCornerCase : public TestUnitWrapper<uint8_t,uint8_t>{
public:
  TestCmnInterruptPushPopTopCornerCase():TestUnitWrapper("test_cmn_isr_mgr_corner_case"){}

  bool run( uint8_t& input, uint8_t& ref) override{
    gInterruptProcess.record_cnt = 0;
    
    uint32_t ret = cmn_interrupt_top();
    if(ret!=UINT32_MAX){
      this->_err_msg << "Empty interrupt queue should return the maximum clock count."<<" dut: top="<<ret<<" ref: top="<<UINT32_MAX<<std::endl;
      return false;
    }

    return true;
  }
};


class TestCmnDummy : public TestUnitWrapper<uint8_t,uint8_t>{
public:
  TestCmnDummy():TestUnitWrapper("test_cmn_dummy"){}

  bool run( uint8_t& input, uint8_t& ref) override{
    return true;
  }
};


class TestCmnMathGcd : public TestUnitWrapper<std::array<std::array<int32_t,2>,10>,std::array<int32_t,10>>{
public:
  TestCmnMathGcd():TestUnitWrapper("test_cmn_math"){}

  bool run( std::array<std::array<int32_t,2>,10>& input, std::array<int32_t,10>& ref) override{
    for( int i=0; i<10; ++i){
      int32_t dut = cmn_math_gcd(input[i][0],input[i][1]);
      if( ref[i]!=dut){
        this->_err_msg<<"Result mismatched. Input=["<<input[i][0]<<','<<input[i][1]<<']'<<" dut="<<dut<<" ref="<<ref[i]<<std::endl;
        return false;
      }
    }
    return true;
  }
};



void add_cmn_test(void){
  tb_infra
    .insert( 
      TestCmnDummy(), 
      0, 
      0
    )

    .insert(
      TestCmnInterruptPushPopTopBasic(),
      std::vector<tCmnInterruptUnit>{
        {.operation=ENABLE,  .irqn=EXTI9_5_IRQn, .target_cnt=11256},
        {.operation=ENABLE,  .irqn=USART1_IRQn,  .target_cnt=8837},
        {.operation=DISABLE, .irqn=SPI3_IRQn,    .target_cnt=47},
        {.operation=DISABLE, .irqn=EXTI0_IRQn,   .target_cnt=203984},
        {.operation=ENABLE,  .irqn=EXTI1_IRQn,   .target_cnt=637}
      },
      std::vector<uint32_t>{47,637,8837,11256,203984}
    )

    .insert(
      TestCmnInterruptPushPopTopCornerCase(),
      (uint8_t)0,
      (uint8_t)0
    )

    .insert( 
      TestCmnMathGcd(),
      std::array<std::array<int32_t,2>,10>{{
        {0,0},
        {4,6},
        {46,78},
        {78,46},
        {78,3},
        {435,134},
        {89327645,4845},
        {139,84435},
        {9834,672},
        {128,1024}
      }}, 
      std::array<int32_t,10>{{
        0,
        2,
        2,
        2,
        3,
        1,
        95,
        1,
        6,
        128
      }}
    )
  ;

}
