// Programing Language SRU
// Copyright(C) 2001-2008 IKeJI
// 

#include "testing.h"

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace sru_test {

void TestCollection::AddTestCase(const string& name, TestCase * test_case){
  tests.insert(make_pair(name,test_case));
}

int TestCollection::RunAllTests(const string& prog){
  int all_tests = tests.size();
  int collect_tests = 0;
  for( map<string,TestCase*>::iterator it = tests.begin();
       it != tests.end();
       it++ ){
    cout << "TEST       :" << it->first << endl;
    int ret = system((prog + " " + it->first).c_str());
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
  if( tests.count(name) == 0 ){
    cout << "test " << name << " not found";
    return -1;
  }
  (*tests[name])();
  return 0;
}

} // namespace sru_test

using namespace std;

int main(int argc, char* argv[]){
  assert(argc > 0);

  // call all test(s)
  if(argc == 1){
    return sru_test::TestCollection::Instance()->RunAllTests(argv[0]);
  }
 
  // exec one test 
  return sru_test::TestCollection::Instance()->RunTest(argv[1]);
}
