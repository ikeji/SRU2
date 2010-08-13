#include <csru/basic_object.h>
#include <csru/native_proc.h>
#include <csru/library.h>
#include <csru/sru_string.h>
#include <iostream>

using namespace sru;
using namespace std;

DEFINE_SRU_PROC(hello){
  cout << "Hello, world" << endl;
  return Library::Instance()->Nil();
}

DEFINE_SRU_PROC(hello2){
  return SRUString::New(symbol("Hello world"));
}

extern "C" bool require(const BasicObjectPtr& env){
  env->Set(symbol("hello"), CREATE_SRU_PROC(hello));
  env->Set(symbol("hello2"), CREATE_SRU_PROC(hello2));
  return true;
};
