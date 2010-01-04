// Programing Language SRU
// Copyright(C) 2005-2009 IKeJI
// 
#include "numeric.h"

#include "basic_object.h"
#include "library.h"
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

int SRUNumeric::GetValue(BasicObjectPtr obj){
  SRUNumeric* n = dynamic_cast<SRUNumeric*>(obj->Data());
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
