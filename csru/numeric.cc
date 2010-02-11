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
  BasicObjectPtr ret = BasicObject::New(new SRUNumeric(value));
  Class::InitializeInstance(ret, Library::Instance()->Numeric());
  return ret;
}

DEFINE_SRU_PROC(NumericParse){
  assert(args.size() >= 2);
  string narg = SRUString::GetValue(args[1]);
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

void SRUNumeric::InitializeClassObject(BasicObjectPtr numeric){
  Class::SetAsSubclass(numeric, NULL);
  numeric->Set("parse",CREATE_SRU_PROC(NumericParse));
  numeric->Set(fNAME, SRUString::New("Numeric"));
  Class::SetAsInstanceMethod(numeric, "equal", CREATE_SRU_PROC(Equal));
  Class::SetAsInstanceMethod(numeric, "notEqual", CREATE_SRU_PROC(NotEqual));
  Class::SetAsInstanceMethod(numeric, "greaterThan", CREATE_SRU_PROC(GreaterThan));
  Class::SetAsInstanceMethod(numeric, "lessThan", CREATE_SRU_PROC(LessThan));
  Class::SetAsInstanceMethod(numeric, "plus", CREATE_SRU_PROC(Plus));
  Class::SetAsInstanceMethod(numeric, "minous", CREATE_SRU_PROC(Minous));
  Class::SetAsInstanceMethod(numeric, "asterisk", CREATE_SRU_PROC(Asterisk));
  Class::SetAsInstanceMethod(numeric, "slash", CREATE_SRU_PROC(Slash));
}

int SRUNumeric::GetValue(BasicObjectPtr obj){
  SRUNumeric* n = obj->GetData<SRUNumeric>();
  if(!n)
    return 0;
  return n->pimpl->value;
}

string SRUNumeric::Inspect(){
  ostringstream o;
  o << "<Numeric: " << pimpl->value << " >";
  return o.str();
}

SRUNumeric::SRUNumeric(int n):
  pimpl(new Impl(n)){
}

SRUNumeric::~SRUNumeric(){
  delete pimpl;
}
