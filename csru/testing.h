// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#ifndef TESTING_H_
#define TESTING_H_

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace sru_test {

using namespace std;

class TestCase;

class TestCollection {
 public:
  static TestCollection * Instance(){
    static TestCollection * instance = new TestCollection();
    return instance;
  }
  void AddTestCase(const string& name, TestCase * test_case);
  int RunAllTests(const string& prog);
  int RunTest(const string& name);
 private:
  map<string,TestCase *>  tests;
};

class TestCase {
 public:
  TestCase(const string& name){
    TestCollection::Instance()->AddTestCase(name,this);
  }
  virtual void operator()() = 0;
};

} // namespace sru_test

#define TEST(name) \
  class TEST_##name : public sru_test::TestCase{ \
   public: \
    TEST_##name(const string& name) : TestCase(name){} \
    void operator()(); \
  } test_##name(#name) ;\
  void TEST_##name::operator()()

#endif // TESTING_H_
