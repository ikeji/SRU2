// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "binding.h"

#include "basic_object.h"
#include "string.h"
#include "class.h"
#include "library.h"
#include "constants.h"

using namespace sru;

void Binding::InitializeClassObject(BasicObjectPtr binding){
  Class::InitializeInstance(binding, Library::Instance()->Class());
  binding->Set(fNAME, SRUString::New("Binding"));
}

BasicObjectPtr Binding::New(const BasicObjectPtr& parent){
  BasicObjectPtr r = BasicObject::New();
  Class::InitializeInstance(r,Library::Instance()->Binding());
  if(parent.get() != NULL)
    r->Set(fPARENT_SCOPE, parent);
  return r;
}

