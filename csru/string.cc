// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "string.h"

#include <string>
#include "basic_object.h"
#include "library.h"
#include "class.h"

using namespace sru;
using namespace std;

struct SRUString::Impl{
  Impl(string value): value(value){}
  string value;
};

BasicObjectPtr SRUString::New(const string& val){
  // TODO: instance-nize
  BasicObjectPtr ret = BasicObject::New(new SRUString(val));
  Class::InitializeInstance(ret,Library::Instance()->String());
  return ret;
}

const string& SRUString::GetValue(BasicObjectPtr obj){
  SRUString* s = obj->GetData<SRUString>();
  static string empty = "";
  if(!s)
    return empty;
  return s->pimpl->value;
}

string SRUString::Inspect(){
  return string("<\"") + pimpl->value + "\">";
}

SRUString::SRUString(const string& val):
      pimpl(new Impl(val)){
}

SRUString::~SRUString(){
  delete pimpl;
}

