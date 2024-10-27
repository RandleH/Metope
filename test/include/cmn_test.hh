


#include "test.hh"
#include "cmn_type.h"
#include "cmn_interrupt.h"


#ifndef CMN_TEST_HH
#define CMN_TEST_HH


/* ************************************************************************** */
/*                         Bindings - TestInterrupt                           */
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



template<class Q, class A>
class TestInterruptPushPopTopBasic : public TestUnitWrapper<Q,A>{
public:
  TestInterruptPushPopTopBasic():TestUnitWrapper<Q,A>("Interrupt Management - Push/Pop/Top - Basic"){}

  bool run( Q& input, A& ref) override{
    this->_is_tested = true;
    
    size_t cnt = 0;
    for(auto &item : input){
      if(false==cmn_interrupt_push(&item)){
        this->_result  = false;
        this->_err_msg << "Failed to insert item "<<cnt<<": target_cnt="<<item.target_cnt<<" irqn="<<item.irqn<<" operation="<<item.operation<<std::endl;
        return false;
      }
      ++cnt;
    }

    if(gInterruptProcess.record_cnt!=input.size()){
      this->_result  = false;
      this->_err_msg << "Record length mismatched!! dut="<<gInterruptProcess.record_cnt << " ref="<<input.size()<<std::endl;
      return false;
    }

    cnt = 0;
    while(gInterruptProcess.record_cnt){
      uint32_t tmp = gInterruptProcess.record_cnt;

      uint32_t top = cmn_interrupt_top();
      if( ref[cnt]!=top){
        this->_result  = false;
        this->_err_msg << "Incorrect top value in the PQ. Processing cnt="<<cnt<<" dut="<<top<<" ref="<<ref[cnt]<<std::endl;
        return false;
      }

      cmn_interrupt_pop();
      if(gInterruptProcess.record_cnt != tmp-1){
        this->_result  = false;
        this->_err_msg << "Count wasn't decreased by 1. Processing cnt="<<cnt<<" dut="<<gInterruptProcess.record_cnt<<" ref="<<(tmp-1)<<std::endl;
        return false;
      }
    }

    this->_result = true;
    return this->_result;
  }
  
  ~TestInterruptPushPopTopBasic(){
  
  };
};


template<class Q, class A>
class TestInterruptPushPopTopCornerCase : public TestUnitWrapper<Q,A>{
public:
  TestInterruptPushPopTopCornerCase():TestUnitWrapper<Q,A>("Interrupt Management - Push/Pop/Top Corner Case"){}

  bool run( Q& input, A& ref) override{
    this->_is_tested = true;

    gInterruptProcess.record_cnt = 0;
    
    uint32_t ret = cmn_interrupt_top();
    if(ret!=UINT32_MAX){
      this->_result  = false;
      this->_err_msg << "Empty interrupt queue should return the maximum clock count."<<" dut: top="<<ret<<" ref: top="<<UINT32_MAX<<std::endl;
    }

    return this->_result;
  }
};









#endif
