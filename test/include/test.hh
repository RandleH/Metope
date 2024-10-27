/**
 ******************************************************************************
 * @file    test.cc
 * @author  RandleH
 * @brief   Testing Program - Pre-Integration CI/Post-Integration CI
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


/* ************************************************************************** */
/*                              Headfile Guards                               */
/* ************************************************************************** */
#ifndef TEST_HH
#define TEST_HH



class TestBone{
protected:
  const std::string _name;

public:
  TestBone() = delete;
  TestBone(const TestBone &x):_name(x._name){}
  TestBone(const std::string test_name):_name(test_name){}
  
  virtual bool run(void* q, void *a) = 0;

  virtual ~TestBone() = default;

  friend std::ostream& operator<<(std::ostream& out, const TestBone& testunit_client);

};
std::ostream& operator<<(std::ostream& out, const TestBone& testunit_client);


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
protected:
  std::stringstream _err_msg;
  bool              _result;
  bool              _is_tested;

public:
  TestUnitWrapper() = delete;
  TestUnitWrapper(const std::string test_name):TestBone(test_name),_err_msg(""),_result(false),_is_tested(false){}
  TestUnitWrapper(const TestUnitWrapper &x):TestBone(x._name),_err_msg(""),_result(false),_is_tested(false){}
  TestUnitWrapper(TestUnitWrapper&& x) noexcept{}

  ~TestUnitWrapper(){
    _err_msg.str("");
  }

  /**
   * @note
   *  Wrapped templated function
  */
  bool run(void *q, void *a) override{
    return run( (*( static_cast<Q*>(q) )), (*( static_cast<A*>(a) )));
  }
  
  /**
   * @note
   *  User Implementation
  */
  virtual bool run(Q& q, A& a) = 0;
};



/**
 * @brief General Test Bench Portal
 * @note  This structure will ONLY iterate your whole test set and run it.
 * 
 * @remark  Import Tests
 * @note    Your test data will be taken as a reference and will not be stored.
 *          Please make sure the entity `Test` and your tests data were in the 
 *          same life time.
 * @example
 * 
 * @remark  Insert Tests
 * @note    Memory will be allocated. Your test data will have a deep copy to 
 *          the buffer.
 * @example
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

  void verdict(void){
    _cout<<"Number of tests in this batch: "<< _package.size()<<std::endl;
    uint32_t cnt = 0;
    for( const auto &item : _package){
      _cout<<"Running test ["<<cnt<<'/'<<_package.size()<<']'<<"...";
      bool result = item.first->run(item.second.first, item.second.second);

      if(result==false){
        _cout<<"FAILED\n"<<std::endl;
        // _cout<<(*pClient)<<std::endl;
      }else{
        _cout<<"PASSED"<<std::endl;
      }
      ++cnt;
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



#endif