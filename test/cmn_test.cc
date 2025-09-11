













#include <bitset>
#include "test.hh"
#include "global.h"

#include "cmn_test.hh"
#include "cmn_type.h"
#include "cmn_math.h"
#include "cmn_interrupt.h"


/* ************************************************************************** */
/*                             Default Namespace                              */
/* ************************************************************************** */


#ifdef __cplusplus
extern "C"{
#endif

extern cmnBoolean_t cmn_interrupt_push( const tCmnInterruptUnit *unit);
extern void cmn_interrupt_pop( void);
extern uint32_t cmn_interrupt_top( void);

#ifdef __cplusplus
}
#endif



class TestCmnInterruptPushPopTopBasic : public TestUnitWrapper<std::vector<tCmnInterruptUnit>,std::vector<uint32_t>>{
public:
  TestCmnInterruptPushPopTopBasic():TestUnitWrapper("test_cmn_isr_mgr_basic"){}

  bool run( std::vector<tCmnInterruptUnit>& input, std::vector<uint32_t>& ref) override{
    size_t cnt = 0;
    for(auto &item : input){
      if(false==cmn_interrupt_push(&item)){
        this->_err_msg << "Failed to insert item "<<cnt<<": target_cnt="<<item.target_cnt<<" irqn="<<item.irqn<<" operation="<<item.operation<<endl;
        return false;
      }
      ++cnt;
    }

    if(gInterruptProcess.record_cnt!=input.size()){
      this->_err_msg << "Record length mismatched!! dut="<<gInterruptProcess.record_cnt << " ref="<<input.size()<<endl;
      return false;
    }

    cnt = 0;
    while(gInterruptProcess.record_cnt){
      uint32_t tmp = gInterruptProcess.record_cnt;

      uint32_t top = cmn_interrupt_top();
      if( ref[cnt]!=top){
        this->_err_msg << "Incorrect top value in the PQ. Processing cnt="<<cnt<<" dut="<<top<<" ref="<<ref[cnt]<<endl;
        return false;
      }

      cmn_interrupt_pop();
      if(gInterruptProcess.record_cnt != tmp-1){
        this->_err_msg << "Count wasn't decreased by 1. Processing cnt="<<cnt<<" dut="<<gInterruptProcess.record_cnt<<" ref="<<(tmp-1)<<endl;
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
      this->_err_msg << "Empty interrupt queue should return the maximum clock count."<<" dut: top="<<ret<<" ref: top="<<UINT32_MAX<<endl;
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
        this->_err_msg<<"Result mismatched. Input=["<<input[i][0]<<','<<input[i][1]<<']'<<" dut="<<dut<<" ref="<<ref[i]<<endl;
        return false;
      }
    }
    return true;
  }
};

class TestCmnMathBit : public TestUnitWrapper<std::array<uint32_t,10>,std::array<uint32_t,10>>{
public:
  TestCmnMathBit():TestUnitWrapper("test_cmn_math_bit"){}

  bool run(std::array<uint32_t,10> &input,std::array<uint32_t,10> &ref) override{
    uint32_t  x8 =input[0], r8;
    r8 = CMN_CLZ_U32(x8);
    if( r8!=ref[0] ){
      
#if USE_LOCAL_IOSTREAM
      /**
       * @todo: Local iostream doesn't support bitset output.
       */
      this->_err_msg<<"Mismatch in CMN_CLZ_U32()."<<" input="<<input[0]<<" dut="<<r8<<" ref="<<ref[0]<<endl;
#else
      this->_err_msg<<"Mismatch in CMN_CLZ_U32()."<<" input="<<"0b"<<std::bitset<32>{input[0]}<<" dut="<<r8<<" ref="<<ref[0]<<endl;
#endif

      return false;
    }
    
    return true;
  }
};


#include "bsp_gyro.h"

namespace paramsTestCmnStructAlignment{
typedef std::array<void*,8> Input;
typedef std::array<void*,8> Output;
}

class TestCmnStructAlignment : public TestUnitWrapper<paramsTestCmnStructAlignment::Input,paramsTestCmnStructAlignment::Output>{
public:
  TestCmnStructAlignment():TestUnitWrapper("test_cmn_struct_alignment"){}
  bool run( paramsTestCmnStructAlignment::Input &i,paramsTestCmnStructAlignment::Output &o ) override{

    if(i.size()!=o.size()){
      this->_err_msg << "Test parameters length mismatched" << "DUT: "<<i.size()<<" REF: "<<o.size()<<endl;
      return false;
    }
    bool result = true;
    for(u8 x=0; x<i.size(); ++x){
      result &= (i[x]==o[x]);
      if(i[x]!=o[x]) {
        this->_err_msg << "DUT: " << i[x] << " REF: " << o[x]<<endl;
        result = false;
      }
    }

    return result;
  }
};

void add_cmn_test(void){
  tb_infra_local
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

    .insert(
      TestCmnMathBit(),
      std::array<uint32_t,10>{
        {0x007B2A52}    /*!< Input: Counting Leading Zero */
      },
      std::array<uint32_t,10>{
        {9}             /*!< Ref: Counting Leading Zero */
      }
    )
    .insert(
      TestCmnStructAlignment(),
      paramsTestCmnStructAlignment::Input{{
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->temperature.L),\
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->temperature.H),\
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->gyro.x),\
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->gyro.y),\
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->gyro.z),\
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->acc.x),\
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->acc.y),\
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->acc.z)
      }},
      paramsTestCmnStructAlignment::Output{{
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->raw[0]),\
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->raw[1]),\
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->fifo[0]),\
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->fifo[2]),\
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->fifo[4]),\
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->fifo[6]),\
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->fifo[8]),\
        static_cast<void*>(&static_cast<tBspGyroData*>(nullptr)->fifo[10])
      }}
    )
  ;
}
