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
#include "string.h"
#include "constants.h"
#include "class.h"

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
  assert(args.size() >= 2);
  string narg = SRUString::GetValue(args[1]).to_str();
  return SRUNumeric::New(atoi(narg.c_str()));
}

DEFINE_SRU_PROC(Equal){
  assert(args.size() >= 2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  if(left == right){
    return Library::Instance()->True();
  }else{
    return Library::Instance()->False();
  }
}

DEFINE_SRU_PROC(NotEqual){
  assert(args.size() >= 2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  if(left == right){
    return Library::Instance()->False();
  }else{
    return Library::Instance()->True();
  }
}

DEFINE_SRU_PROC(GreaterThan){
  assert(args.size() >= 2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  if(left > right){
    return Library::Instance()->True();
  }else{
    return Library::Instance()->False();
  }
}

DEFINE_SRU_PROC(LessThan){
  assert(args.size() >= 2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  if(left < right){
    return Library::Instance()->True();
  }else{
    return Library::Instance()->False();
  }
}


DEFINE_SRU_PROC(Plus){
  assert(args.size() >= 2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  return SRUNumeric::New(left+right);
}

DEFINE_SRU_PROC(Minous){
  assert(args.size() >= 2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  return SRUNumeric::New(left-right);
}

DEFINE_SRU_PROC(Asterisk){
  assert(args.size() >= 2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  return SRUNumeric::New(left*right);
}

DEFINE_SRU_PROC(Slash){
  assert(args.size() >= 2);
  const int& left = SRUNumeric::GetValue(args[0]);
  const int& right = SRUNumeric::GetValue(args[1]);
  return SRUNumeric::New(left/right);
}

DEFINE_SRU_PROC(Invert){
  assert(args.size() >= 1);
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
}

int SRUNumeric::GetValue(const BasicObjectPtr& obj){
  SRUNumeric* n = obj->GetData<SRUNumeric>();
  if(!n)
    return 0;
  return n->pimpl->value;
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
