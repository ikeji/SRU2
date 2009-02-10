// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#ifndef TESTING_H_
#define TESTING_H_

#include <string>
#include <utility>
#include <vector>

namespace sru_test {

class TestCase;

// Collect and process tests.
class TestCollection {
 public:
  // TestCollection is singleton.
  static TestCollection * Instance(){
    static TestCollection instance;
    return &instance;
  }

  void AddTestCase(const std::string& name, TestCase * test_case);
  int RunAllTests(const std::string& prog);
  int RunTest(const std::string& name);
 private:
  TestCollection();
  virtual ~TestCollection();

  struct Impl;
  Impl* pimpl;

  TestCollection(const TestCollection& obj);
  TestCollection& operator=(const TestCollection& obj);
};

// Base class for each test instane.
// This use in TEST macro.
class TestCase {
 public:
  TestCase(const std::string& name){
    // If I make instance of TestCase. The case add to TestCollection.
    TestCollection::Instance()->AddTestCase(name,this);
  }
  virtual void operator()() = 0;
};

} // namespace sru_test

// Convinience macro for tests.
// Example:
//   TEST(SampleTest){
//     // test code.
//   }
//
// You can use assert(2) or exit(3) inside test.
//
#define TEST(name) \
  class TEST_##name : public sru_test::TestCase{ \
   public: \
    TEST_##name(const std::string& name) : TestCase(name){} \
    void operator()(); \
  } test_##name(#name) ;\
  void TEST_##name::operator()()

#endif // TESTING_H_
