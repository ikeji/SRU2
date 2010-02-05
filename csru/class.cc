// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "class.h"

#include "basic_object.h"
#include "constants.h"
#include "library.h"
#include "string.h"

namespace sru {

void Class::InitializeInstance(BasicObjectPtr obj,BasicObjectPtr klass){
  obj->Set(fCLASS,klass);
  // TODO: InsertClassSystem
}

void Class::SetAsSubclass(BasicObjectPtr obj,BasicObjectPtr klass){
  if(klass.get() == NULL) klass = Library::Instance()->Object();
  InitializeInstance(obj, Library::Instance()->Class());
  obj->Set(fSUPERCLASS, klass);
}

void Class::InitializeClassClass(BasicObjectPtr klass){
  SetAsSubclass(klass,NULL);
  klass->Set(fNAME, SRUString::New("Class"));
}


}  // namespace sru
