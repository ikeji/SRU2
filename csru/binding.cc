// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "binding.h"

#include "basic_object.h"
#include "string.h"
#include "class.h"
#include "library.h"

using namespace sru;

void Binding::InitializeClassObject(BasicObjectPtr binding){
  Class::InitializeInstance(binding, Library::Instance()->Class());
  binding->Set(fNAME, SRUString::New("Binding"));
}

