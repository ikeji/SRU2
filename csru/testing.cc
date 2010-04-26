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
  Impl():tests(){}
  map<string,TestCase *> tests;
};

TestCollection::TestCollection():pimpl(new Impl()){
}

TestCollection::~TestCollection(){
  delete pimpl;
}

void TestCollection::AddTestCase(const string& name,TestCase* test_case){
  assert(pimpl->tests.find(name) == pimpl->tests.end());
  pimpl->tests.insert(make_pair(name,test_case));
}

int TestCollection::RunAllTests(const string& prog){
  vector<string> fail_tests;

  // For all tests.
  for( map<string,TestCase*>::iterator it = pimpl->tests.begin();
       it != pimpl->tests.end();
       it++ ){
    cout << "TEST       : " << it->first << endl;

    // Execute test as child process.
    // So we can continue to test at assert or other runtime error.
    int ret = system(("\"" + prog + "\" " + it->first).c_str());
    // check test result in proess result code.
    if(ret == 0){
      cout << "TEST PASSED: " << it->first << endl;
    } else {
      cout << "TEST FAILED: " << it->first << endl;
      fail_tests.push_back(it->first);
    }
  }
  if(fail_tests.empty()){
    cout << "----- ALL TESTS PASSED -----" << endl;
  }else{
    cout << "FAILED: ";
    for(vector<string>::iterator it = fail_tests.begin();
        it != fail_tests.end();
        it++){
      cout << *it << " ";
    }
    cout << endl;
    cout << "----- " << fail_tests.size() << "/" <<
        pimpl->tests.size() << " TEST(S) FAILED!!!! -----" << endl;
  }
  
  return(- fail_tests.size());
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

#include "logging.h"

int main(int argc, char* argv[]){
  assert(argc > 0);

  // Call all test(s) without any argv
  if(argc == 1){
    return sru_test::TestCollection::Instance()->RunAllTests(argv[0]);
  }
 
  // Exec one test with argv
  return sru_test::TestCollection::Instance()->RunTest(argv[1]);
}

