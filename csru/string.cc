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

struct SRUString::Impl{
  Impl(symbol value): value(value){}
  symbol value;
};

BasicObjectPtr SRUString::New(const symbol& val){
  // TODO: instance-nize
  BasicObjectPtr ret = BasicObject::New(new SRUString(val));
  Class::InitializeInstance(ret,Library::Instance()->String());
  return ret;
}

const symbol& SRUString::GetValue(BasicObjectPtr obj){
  SRUString* s = obj->GetData<SRUString>();
  static symbol empty("");
  if(!s)
    return empty;
  return s->pimpl->value;
}

string SRUString::Inspect(){
  return string("<\"") + pimpl->value.to_str() + "\">";
}

SRUString::SRUString(const symbol& val):
      pimpl(new Impl(val)){
}

SRUString::~SRUString(){
  delete pimpl;
}

