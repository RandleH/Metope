/**
 ******************************************************************************
 * @file    test.hh
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
#include <cstdbool>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <sys/stat.h>
#include <memory>
#include <queue>
#include <cstring>
#include <iomanip>


/* ************************************************************************** */
/*                              Headfile Guards                               */
/* ************************************************************************** */
#ifndef TEST_HH
#define TEST_HH


/* ************************************************************************** */
/*                                  Macros                                    */
/* ************************************************************************** */
#define MAX_TEST_NAME_LENGTH       64
#define CONSOLE_FMT_RED           "\033[31m"
#define CONSOLE_FMT_YELLOW        "\033[33m"
#define CONSOLE_FMT_BLUE          "\033[44m"
#define CONSOLE_FMT_ITALIC        "\033[23m"
#define CONSOLE_FMT_BOLD          "\033[1m"
#define CONSOLE_FMT_RESET         "\033[0m"
#define CONSOLE_FMT_USER_CUSTOM_1 "\033[38;5;51m"
#define CONSOLE_FMT_USER_CUSTOM_2 "\033[48;5;51m"


/* ************************************************************************** */
/*                            Test Bone Component                             */
/* ************************************************************************** */
/**
 * @brief Test Bench Bone Structure (Do NOT modify/inherit)
 * @note  Please develop the test on class `TestUnitWrapper`
*/
class TestBone{
protected:
  char _name[MAX_TEST_NAME_LENGTH];

public:
  TestBone() = delete;
  TestBone(const TestBone &x):_name{0}{
    stpncpy(_name, x._name, MAX_TEST_NAME_LENGTH-1);
  }
  TestBone(const std::string test_name):_name{0}{
    stpncpy(_name, test_name.c_str(), MAX_TEST_NAME_LENGTH-1);
  }

  const char* name(void){return _name;}

  virtual inline const char* info(void) const noexcept = 0;
  virtual              bool  run(void* q, void *a)     = 0;
  virtual                    ~TestBone()               = default;
  friend std::ostream& operator<<(std::ostream& out, const TestBone& testunit_client);
};
std::ostream& operator<<(std::ostream& out, const TestBone& testunit_client);



/* ************************************************************************** */
/*                       Test Bone Wrapper Component                          */
/* ************************************************************************** */
/**
 * @brief General Test Bench Unit
 * @note  User needs to implement the `run()` function. This is an abstract class
 *        Type <Q> is your test input data and type <A> is your test answer.
 *        Typically they are a vector array.
 * 
 * @tparam [Q] - The input data of your tests
 * @tparam [A] - The expected answer of your tests
 * 
 * @author Randle.Helmslay [Signed]
*/
template<class Q, class A>
class TestUnitWrapper : public TestBone{
private:
  bool              _result;
  bool              _is_tested;

protected:
  std::stringstream _err_msg;

public:
  TestUnitWrapper() = delete;
  TestUnitWrapper(const std::string test_name):TestBone(test_name),_result(false),_is_tested(false),_err_msg(""){}
  TestUnitWrapper(const TestUnitWrapper &x):TestBone(x._name),_result(false),_is_tested(false),_err_msg(""){}
  TestUnitWrapper(TestUnitWrapper&& x) noexcept{}
  
  inline const char* info(void) const noexcept{
    return  ("\t"+_err_msg.str()).c_str();
  }

  ~TestUnitWrapper(){
    _err_msg.str("");
  }

  /**
   * @note
   *  Wrapped templated function
  */
  bool run(void *q, void *a) override{
    _is_tested = true;
    _result    = run( (*( static_cast<Q*>(q) )), (*( static_cast<A*>(a) )));
    return _result;
  }
  
  /**
   * @note
   *  User Implementation
  */
  virtual bool run(Q& q, A& a) = 0;
};


/**
 * @brief General Test Bench Unit with Input(`istream`)
 * 
 * @author Randle.Helmslay [Signed]
*/
template<class Q, class A>
class TestUnitWrapper_withInput : public TestUnitWrapper<Q,A>{
protected:
  std::istream& _cin;
public:
  TestUnitWrapper_withInput() = delete;
  TestUnitWrapper_withInput(const std::string test_name, std::istream& cin):TestUnitWrapper<Q,A>(test_name),_cin(cin){}
  TestUnitWrapper_withInput(const TestUnitWrapper_withInput &x):TestUnitWrapper<Q,A>(x),_cin(x._cin){}

  ~TestUnitWrapper_withInput(){}
};


/**
 * @brief General Test Bench Unit with Output(`ostream`)
 * 
 * @author Randle.Helmslay [Signed]
*/
template<class Q, class A>
class TestUnitWrapper_withOutput : public TestUnitWrapper<Q,A>{
protected:
  std::ostream& _cout;
public:
  TestUnitWrapper_withOutput() = delete;
  TestUnitWrapper_withOutput(const std::string test_name, std::ostream& cout):TestUnitWrapper<Q,A>(test_name),_cout(cout){}
  TestUnitWrapper_withOutput(const TestUnitWrapper_withOutput &x):TestUnitWrapper<Q,A>(x),_cout(x._cout){}

  ~TestUnitWrapper_withOutput(){}
};


/**
 * @brief General Test Bench Unit with Input and Output(`iostream`)
 * 
 * @author Randle.Helmslay [Signed]
*/
template<class Q, class A>
class TestUnitWrapper_withInputOutput : public TestUnitWrapper<Q,A>{
protected:
  std::istream& _cin;
  std::ostream& _cout;
public:
  TestUnitWrapper_withInputOutput() = delete;
  TestUnitWrapper_withInputOutput(const std::string test_name, std::istream& cin, std::ostream& cout):TestUnitWrapper<Q,A>(test_name),_cin(cin),_cout(cout){}
  TestUnitWrapper_withInputOutput(const TestUnitWrapper_withInputOutput &x):TestUnitWrapper<Q,A>(x),_cin(x._cin),_cout(x._cout){}
  ~TestUnitWrapper_withInputOutput(){}
};


/* ************************************************************************** */
/*                            Test Infrastructure                             */
/* ************************************************************************** */
/**
 * @brief General Test Bench Portal
 * @note  This structure will ONLY iterate your whole test set and run it.
 * 
 * @remark  Import Tests
 * @note    Your test data will be taken as a reference and will not be stored.
 *          Please make sure the entity `Test` and your tests data were in the 
 *          same life time.
 * 
 * @remark  Insert Tests
 * @note    Memory will be allocated. Your test data will have a deep copy to 
 *          the buffer.
 * 
 * @example 
 *          
 *          template<class Q, class A>
 *          class YourTestClass : public TestUnitWrapper<Q,A>{
 *          public:
 *            YourTestClass() : TestUnitWrapper<Q,A>("Your Test Description"){}
 *            
 *            bool run( Q& input, A& ref) override{
 *              // Your Test Body //
 *            }
 *          };
 *          
 *          // Prepare the test material
 *          TypeTestInput  input[]  = {x,x,x,x,x,x,x};
 *          TypeTestResult result[] = {y,y,y,y,y,y,y};
 * 
 *          // Method 1 - Run Immediately
 *          YourTestClass< typeof(input), typeof(result) >() client; // Instantiate the test client
 *          client.run( input, result );                             // Run the test
 * 
 *          // Method 2 - Use Test Manager (Recommanded for CI)
 *          Test tb_infra(std::cout);                                // Set `std::cout` as the output stream
 *          
 *            // Method 2.1 - User takes care of the test data memory
 *            YourTestClass< typeof(input), typeof(result) >() client; // Instantiate the test client
 *            tb_infra.import( client, input, result);
 * 
 *            // Method 2.1 - TB takes care of the test data memory
 *            tb_infra.insert(YourTestClass< typeof(input), typeof(result) >(), input, result);
 *            input[...]  = ...                                        // Change some input set
 *            result[...] = ...                                        // Change some result set
 *            tb_infra.insert(YourTestClass< typeof(input), typeof(result) >(), input, result);
 *          
 * 
 * @author Randle.Helmslay [Signed]
*/
class Test{
private:
  std::map<TestBone*, std::pair<void*,void*>> _package;
  std::queue<TestBone*>                       _alloc;
  std::ostream&                               _cout;

public:
  Test(std::ostream& ostream):_cout(ostream){}

  Test& import( TestBone *client, void *Q_input, void *A_answer){
    _package.insert(std::make_pair( client, std::make_pair(Q_input, A_answer)));
    return (*this);
  }

  template<class T, class Q, class A>
  Test& insert( const T& client, const Q& input, const A& answer){
    auto *pT = new T(std::move(client));
    auto *pQ = new Q(std::move(input));
    auto *pA = new A(std::move(answer));
    _alloc.push(pT);
    
    _package.insert(std::make_pair( pT, std::make_pair(pQ, pA)));
    return (*this);
  }

  virtual void callback_if_passed( void){}
  virtual void callback_if_failed( void){}

  void verdict(void){
    _cout<<"Number of tests in this batch: "<< _package.size()<<'\n'<<std::endl;
    uint32_t cnt = 0;
    bool     v_result = true;
    for( const auto &item : _package){
      _cout<<"Running test ["<<cnt+1<<'/'<<_package.size()<<']'<<' '<< std::setfill('.') << std::left << std::setw(MAX_TEST_NAME_LENGTH) << item.first->name()<<' ';
      bool result = item.first->run(item.second.first, item.second.second);
      v_result &= result;
      if(result==false){
        _cout<<"FAILED"<<std::endl;
        _cout<<CONSOLE_FMT_RED<<"Error Signature:\n"<<item.first->info()<<CONSOLE_FMT_RESET<<std::endl;
      }else{
        _cout<<"PASSED"<<std::endl;
      }
      ++cnt;
    }

    _cout<<"\nTest completed. Verdict result: "<< ((v_result==false) ? "FAILED" : "PASSED")<<"\n"<<std::endl;

    if(v_result==false){
      callback_if_failed();
    }else{
      callback_if_passed();
    }
  }
  

  /**
   * @todo
   *    Check if this is valid.
  */
  ~Test(){
    while( !_alloc.empty()){
      auto *pT = _alloc.front();
      auto *pQ = _package[pT].first;
      auto *pA = _package[pT].second;
      delete pT;
      delete [] (char*)pQ;
      delete [] (char*)pA;
      _alloc.pop();
    }
  }

};




/* ************************************************************************** */
/*                       Customized Test Infrastructure                       */
/* ************************************************************************** */
#include "bsp_led.h"    // Required by class `LocalProjectTest`
/**
 * @brief Local Test Infrastructure
 * @note  Test for local debugging
 */
class LocalProjectTest : public Test{
public:
  using Test::Test;
  void callback_if_passed(void) override{bsp_led__on();}
  void callback_if_failed(void) override{bsp_led__off();}
};


/**
 * @brief Interactional Test Infrastructure
 * @note  Test process may need mannual intervention
 */
class HumanInteractionTest : public Test{
protected:
  
public:
  using Test::Test;
};


/**
 * @brief CI Test Infrastructure
 * @note  Test for continueous integration
 */
class IntegrationTest : public Test{
public:
  using Test::Test;
  void callback_if_passed(void) override{
    /**
     * @todo
     *  Generate a system interrupt with a ERRNO status
     */
  }
  void callback_if_failed(void) override{
    /**
     * @todo
     *  Generate a system interrupt with a EINTR status
     */
  }
};


#endif