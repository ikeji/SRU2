// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "string.h"

#include <string>
#include "basic_object.h"
#include "library.h"
#include "class.h"
#include "symbol.h"

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
