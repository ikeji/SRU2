// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace sru_test;
using namespace std;

struct TestCollection::Impl {
  map<string,TestCase *>  tests;
};

TestCollection::TestCollection() {
  pimpl = new Impl();
}

TestCollection::~TestCollection(){
  delete pimpl;
}

void TestCollection::AddTestCase(const string& name, TestCase * test_case){
  pimpl->tests.insert(make_pair(name,test_case));
}

int TestCollection::RunAllTests(const string& prog){
  // Number of all tests.
  const int all_tests = pimpl->tests.size();
  int collect_tests = 0;

  // For all tests.
  for( map<string,TestCase*>::iterator it = pimpl->tests.begin();
       it != pimpl->tests.end();
       it++ ){
    cout << "TEST       :" << it->first << endl;

    // Execute test as child process.
    // So we can continue to test at assert or other runtime error.
    int ret = system((prog + " " + it->first).c_str());
    // check test result in proess result code.
    if(ret == 0){
      collect_tests++;
      cout << "TEST PASSED:" << it->first << endl;
    } else {
      cout << "TEST FAILED:" << it->first << endl;
    }
  }
  if(all_tests == collect_tests)
    cout << "ALL TESTS PASSED" << endl;
  else
    cout << (all_tests - collect_tests) << "/" << all_tests << " TEST(S) FAILED!!!!" << endl;
  return collect_tests - all_tests;
}

int TestCollection::RunTest(const string& name){
  if( pimpl->tests.count(name) == 0 ){
    cout << "test " << name << " not found";
    return -1;
  }
  // Execute a test.
  (*pimpl->tests[name])();

  // Normal exit.
  return 0;
}

int main(int argc, char* argv[]){
  assert(argc > 0);

  // Call all test(s) without any argv
  if(argc == 1){
    return sru_test::TestCollection::Instance()->RunAllTests(argv[0]);
  }
 
  // Exec one test with argv
  return sru_test::TestCollection::Instance()->RunTest(argv[1]);
}

