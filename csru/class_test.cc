// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"

#include "class.h"

#include <cassert>
#include "basic_object.h"
#include "constants.h"
#include "library.h"

using namespace std;
using namespace sru;

TEST(Class_InitializeInstanceTest){
  BasicObjectPtr klass = BasicObject::New();
  BasicObjectPtr instance = BasicObject::New();
  Class::InitializeInstance(instance, klass);
  assert(instance->Get(fCLASS) == klass);
}

TEST(Class_NewClassTest){
  BasicObjectPtr klass = BasicObject::New();
  Class::SetAsSubclass(klass, NULL);
  assert(klass->Get(fCLASS) == Library::Instance()->Class());
  assert(klass->Get(fSUPERCLASS) == Library::Instance()->Object());
}

TEST(Class_SubClassTest){
  BasicObjectPtr klass = BasicObject::New();
  BasicObjectPtr subclass = BasicObject::New();
  Class::SetAsSubclass(subclass, klass);
  assert(subclass->Get(fCLASS) == Library::Instance()->Class());
  assert(subclass->Get(fSUPERCLASS) == klass);
}

TEST(Class_InstanceMethodTest){
  BasicObjectPtr klass = BasicObject::New();
  BasicObjectPtr method = BasicObject::New();
  Class::SetAsInstanceMethod(klass, "AMethod", method);
  assert(klass->HasSlot(fINSTANCE_METHODS));
  // TODO: Test field named instance method has "AMethod".
}

TEST(Class_InitializeClassTest){
  BasicObjectPtr klass = Library::Instance()->Class();
  assert(klass.get());
  assert(klass->HasSlot(fCLASS));
  assert(klass->Get(fCLASS) == klass);
  assert(klass->HasSlot(fSUPERCLASS));
  assert(klass->Get(fSUPERCLASS) == Library::Instance()->Object());
}
