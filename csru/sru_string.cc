// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
#include "sru_string.h"

#include <string>
#include "basic_object.h"
#include "library.h"
#include "class.h"
#include "symbol.h"
#include "constants.h"
#include "native_proc.h"
#include "logging.h"
#include "utils.h"

using namespace sru;
using namespace std;

BasicObjectPtr SRUString::New(const symbol& val){
  // TODO: instance-nize
  const BasicObjectPtr ret = BasicObject::New(new SRUString(val));
  Class::InitializeInstance(ret,Library::Instance()->String());
  return ret;
}

const symbol& SRUString::GetValue(const BasicObjectPtr& obj){
  SRUString* s = obj->GetData<SRUString>();
  return SRUString::GetValue(s);
}

const symbol& SRUString::GetValue(const SRUString* s){
  static symbol empty("");
  if(!s)
    return empty;
  return s->value;
}

string SRUString::Inspect(){
  return string("String(\"") + value.to_str() + "\")";
}

SRUString::SRUString(const symbol& val):
      value(val){
}

void SRUString::Dispose(){
  delete this;
}

namespace {

DEFINE_SRU_PROC(ToS){
  ARGLEN(1);
  return args[0];
}

DEFINE_SRU_PROC(StringEqual){
  ARGLEN(2);
  SRUString* left = args[0]->GetData<SRUString>();
  SRUString* right = args[1]->GetData<SRUString>();
  if(!left || !right) return BooleanToObject(false);
  return BooleanToObject(SRUString::GetValue(left).to_str() ==
                         SRUString::GetValue(right).to_str());
}

DEFINE_SRU_PROC(StringNotEqual){
  ARGLEN(2);
  SRUString* left = args[0]->GetData<SRUString>();
  SRUString* right = args[1]->GetData<SRUString>();
  if(!left || !right) return BooleanToObject(true);
  return BooleanToObject(SRUString::GetValue(left).to_str() !=
                         SRUString::GetValue(right).to_str());
}

}  // anonymous namespace

void SRUString::InitializeStringClass(const BasicObjectPtr& str){
  Class::SetAsSubclass(str, Library::Instance()->Object());
  str->Set(sym::__name(), SRUString::New(sym::String()));
  Class::SetAsInstanceMethod(str, sym::toS(), CREATE_SRU_PROC(ToS));
  Class::SetAsInstanceMethod(str, sym::equal(), CREATE_SRU_PROC(StringEqual));
  Class::SetAsInstanceMethod(str, sym::notEqual(), CREATE_SRU_PROC(StringNotEqual));
}
