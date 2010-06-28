#include <basic_object.h>
#include <native_proc.h>
#include <library.h>
#include <iostream>

using namespace sru;
using namespace std;

DEFINE_SRU_PROC(hello){
  cout << "Hello, world" << endl;
  return Library::Instance()->Nil();
}

extern "C" bool require(const BasicObjectPtr& env){
  env->Set(symbol("hello"), CREATE_SRU_PROC(hello));
  return true;
};
