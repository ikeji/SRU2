// Programing Language SRU
// Copyright(C) 2005-2009 IKeJI
// 
#include "numeric.h"

#include <cstdlib>
#include <string>
#include <sstream>
#include "basic_object.h"
#include "class.h"
#include "library.h"
#include "native_proc.h"
#include "sru_string.h"
#include "constants.h"
#include "class.h"
#include "utils.h"
#include "interpreter.h"
#include "stack_frame.h"
#include "binding.h"
#include "testing_ast.h"
#include "testing_sru.h"
using namespace sru_test;

using namespace sru;
using namespace std;

struct SRUNumeric::Impl{
  Impl(int value): value(value){}
  int value;
};

BasicObjectPtr SRUNumeric::New(int value){
  const BasicObjectPtr ret = BasicObject::New(new SRUNumeric(value));
  Class::InitializeInstance(ret, Library::Instance()->Numeric());
  return ret;
}

DEFINE_SRU_PROC(NumericParse){
  ARGLEN(2);
  string narg = SRUString::GetValue(args[1]).to_str();
  return SRUNumeric::New(atoi(narg.c_str()));
}

DEFINE_SRU_PROC(Equal){
  ARGLEN(2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  if(left == right){
    return Library::Instance()->True();
  }else{
    return Library::Instance()->False();
  }
}

DEFINE_SRU_PROC(NotEqual){
  ARGLEN(2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  if(left == right){
    return Library::Instance()->False();
  }else{
    return Library::Instance()->True();
  }
}

DEFINE_SRU_PROC(GreaterThan){
  ARGLEN(2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  if(left > right){
    return Library::Instance()->True();
  }else{
    return Library::Instance()->False();
  }
}

DEFINE_SRU_PROC(LessThan){
  ARGLEN(2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  if(left < right){
    return Library::Instance()->True();
  }else{
    return Library::Instance()->False();
  }
}


DEFINE_SRU_PROC(Plus){
  ARGLEN(2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  return SRUNumeric::New(left+right);
}

DEFINE_SRU_PROC(Minous){
  ARGLEN(2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  return SRUNumeric::New(left-right);
}

DEFINE_SRU_PROC(Asterisk){
  ARGLEN(2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  return SRUNumeric::New(left*right);
}

DEFINE_SRU_PROC(Slash){
  ARGLEN(2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  return SRUNumeric::New(left/right);
}

DEFINE_SRU_PROC_SMASH(_times_internal){
  const BasicObjectPtr& binding =
    Interpreter::Instance()->CurrentStackFrame()->Binding();
  const BasicObjectPtr& ip = binding->Get(sym::_i());
  if(IsNil(ip)){
    PushResult(Library::Instance()->Nil());
    return;
  }
  const BasicObjectPtr& jp = binding->Get(sym::_j());
  if(IsNil(ip)){
    PushResult(Library::Instance()->Nil());
    return;
  }
  int i = SRUNumeric::GetValue(ip) + 1;
  int j = SRUNumeric::GetValue(jp);
  LOG_TRACE << "i:" << i;
  LOG_TRACE << "j:" << j;
  if( j <= i ) {
    PushResult(Library::Instance()->Nil());
    return;
  }
  binding->Set(sym::_i(), SRUNumeric::New(i));

  Interpreter::Instance()->CurrentStackFrame()->Setup(
      A(
        C(R(sym::_block()), R(sym::_i())),
        C(R(sym::_times_internal()))
      ));
}

DEFINE_SRU_PROC_SMASH(Times){
  static BasicObjectPtr times_internal;
  ARGLEN(2);
  LOG_TRACE << args[0]->Inspect();
  const int& self = SRUNumeric::GetValue(args[0]);
  CHECK(self >= 0) << "Invalid value for time.";
  if(self <= 0){
    PushResult(Library::Instance()->Nil());
    return;
  }
  
  if(times_internal == NULL)
    times_internal = CREATE_SRU_PROC(_times_internal);

  const BasicObjectPtr binding = Binding::New();
  binding->Set(sym::_i(), SRUNumeric::New(-1));
  binding->Set(sym::_j(), args[0]);
  binding->Set(sym::_times_internal(), times_internal);
  binding->Set(sym::_block(), args[1]);
  Interpreter::Instance()->DigIntoNewFrame(
      A(C(R(sym::_times_internal()))),
      binding);
}

DEFINE_SRU_PROC(Invert){
  ARGLEN(1);
  const int& v = SRUNumeric::GetValue(args[0]);
  return SRUNumeric::New(-v);
}

void SRUNumeric::InitializeClassObject(const BasicObjectPtr& numeric){
  Class::SetAsSubclass(numeric, NULL);
  numeric->Set(sym::parse(),CREATE_SRU_PROC(NumericParse));
  numeric->Set(sym::__name(), SRUString::New(symbol("Numeric")));
  Class::SetAsInstanceMethod(numeric, sym::equal(), CREATE_SRU_PROC(Equal));
  Class::SetAsInstanceMethod(numeric, sym::notEqual(), CREATE_SRU_PROC(NotEqual));
  Class::SetAsInstanceMethod(numeric, sym::greaterThan(), CREATE_SRU_PROC(GreaterThan));
  Class::SetAsInstanceMethod(numeric, sym::lessThan(), CREATE_SRU_PROC(LessThan));
  Class::SetAsInstanceMethod(numeric, sym::plus(), CREATE_SRU_PROC(Plus));
  Class::SetAsInstanceMethod(numeric, sym::minus(), CREATE_SRU_PROC(Minous));
  Class::SetAsInstanceMethod(numeric, sym::asterisk(), CREATE_SRU_PROC(Asterisk));
  Class::SetAsInstanceMethod(numeric, sym::slash(), CREATE_SRU_PROC(Slash));
  Class::SetAsInstanceMethod(numeric, sym::invert(), CREATE_SRU_PROC(Invert));

  Class::SetAsInstanceMethod(numeric, sym::times(), CREATE_SRU_PROC(Times));
}

int SRUNumeric::GetValue(const BasicObjectPtr& obj){
  SRUNumeric* n = obj->GetData<SRUNumeric>();
  if(!n) return 0;
  return n->pimpl->value;
}
  
bool SRUNumeric::TryGetValue(const BasicObjectPtr& obj, int* val){
  SRUNumeric* n = obj->GetData<SRUNumeric>();
  if(!n) return false;
  *val = n->pimpl->value;
  return true;
}

string SRUNumeric::Inspect(){
  ostringstream o;
  o << "Numeric(" << pimpl->value << ")";
  return o.str();
}

SRUNumeric::SRUNumeric(int n):
  pimpl(new Impl(n)){
}

SRUNumeric::~SRUNumeric(){
  delete pimpl;
}

void SRUNumeric::Dispose(){
  delete this;
}
