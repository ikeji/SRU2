// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
#include "sru_string.h"

#include <string>
#include "basic_object.h"
#include "library.h"
#include "class.h"
#include "numeric.h"
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

DEFINE_SRU_PROC(StringGet){
  ARGLEN(2);
  // TODO: impliment str[1,2]
  // TODO: impliment str[1..3]
  string str = SRUString::GetValue(args[0]).to_str();
  int index = 0;
  DCHECK(SRUNumeric::TryGetIntValue(args[1], &index))
      << "String.get requires numeric but it was "
      << args[1]->Inspect();
  if((size_t)index >= str.size()) return SRUString::New(symbol(""));
  return SRUString::New(symbol(str.substr(index, 1)));
}

}  // anonymous namespace

void SRUString::InitializeStringClass(const BasicObjectPtr& str){
  Class::SetAsSubclass(str, Library::Instance()->Object());
  str->Set(sym::__name(), SRUString::New(sym::String()));

#define DEFMETHOD(name, proc) \
  Class::SetAsInstanceMethod(str, sym::name(), CREATE_SRU_PROC(proc))

  DEFMETHOD(toS, ToS);
  DEFMETHOD(equal, StringEqual);
  DEFMETHOD(notEqual, StringNotEqual);
  DEFMETHOD(get, StringGet);
}
