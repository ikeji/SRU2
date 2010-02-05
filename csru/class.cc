// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "class.h"

#include <string>
#include "basic_object.h"
#include "constants.h"
#include "library.h"
#include "string.h"

using namespace sru;
using namespace std;

void Class::InitializeInstance(const BasicObjectPtr& obj,
                               const BasicObjectPtr& klass){
  obj->Set(fCLASS,klass);
  // TODO: InsertClassSystem
}

void Class::SetAsSubclass(const BasicObjectPtr& obj,
                          const BasicObjectPtr& super_klass){
  BasicObjectPtr klass = super_klass;
  if(klass.get() == NULL) klass = Library::Instance()->Object();
  InitializeInstance(obj, Library::Instance()->Class());
  obj->Set(fSUPERCLASS, klass);
}

void Class::SetAsInstanceMethod(const BasicObjectPtr& klass,
                                const string& name,
                                const BasicObjectPtr& method){
  if(! klass->HasSlot(fINSTANCE_METHODS))
    // TODO: Use Map class here!
    klass->Set(fINSTANCE_METHODS,BasicObject::New());
  klass->Get(fINSTANCE_METHODS)->Set(name, method);
}

void Class::InitializeClassClass(const BasicObjectPtr& klass){
  SetAsSubclass(klass,NULL);
  klass->Set(fNAME, SRUString::New("Class"));
}
