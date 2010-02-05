// Programing Language SRU
// Copyright(C) 2005-2009 IKeJI
// 
#include "numeric.h"

#include <cstdlib>
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
  assert(arg.size() >= 2);
  string narg = SRUString::GetValue(arg[1]);
  return SRUNumeric::New(atoi(narg.c_str()));
}

void SRUNumeric::InitializeClassObject(BasicObjectPtr numeric){
  Class::SetAsSubclass(numeric, NULL);
  numeric->Set("parse",CREATE_SRU_PROC(NumericParse));
  numeric->Set(fNAME, SRUString::New("Numeric"));
}

int SRUNumeric::GetValue(BasicObjectPtr obj){
  SRUNumeric* n = obj->GetData<SRUNumeric>();
  if(!n)
    return 0;
  return n->pimpl->value;
}

SRUNumeric::SRUNumeric(int n):
  pimpl(new Impl(n)){
}

SRUNumeric::~SRUNumeric(){
  delete pimpl;
}
