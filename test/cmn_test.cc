

















#include "cmn_test.hh"
#include "cmn_type.h"


void foo(void){
  std::vector<tCmnInterruptUnit> input = {
    {.operation=ENABLE,  .irqn=EXTI9_5_IRQn, .target_cnt=11256},
    {.operation=ENABLE,  .irqn=USART1_IRQn,  .target_cnt=8837},
    {.operation=DISABLE, .irqn=SPI3_IRQn,    .target_cnt=47},
    {.operation=DISABLE, .irqn=EXTI0_IRQn,   .target_cnt=203984},
    {.operation=ENABLE,  .irqn=EXTI1_IRQn,   .target_cnt=637}
  };
  std::vector<uint32_t> ref = {47,637,8837,11256,203984};


  TestInterruptPushPopTopBasic<typeof(input), typeof(ref)> a;


  Test tb_infra(std::cout);

  tb_infra
    .import(&a, &input, &ref)
    .import(new TestInterruptPushPopTopBasic<typeof(input), typeof(ref)>(), new std::vector<tCmnInterruptUnit>(input), new std::vector<uint32_t>(ref))
    .import(new TestInterruptPushPopTopCornerCase<char, char>()            , NULL                                     , NULL);





}
