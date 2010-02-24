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
  klass->Set(sym::klass(), Library::Instance()->Class());
  BasicObjectPtr instance = BasicObject::New();
  Class::InitializeInstance(instance, klass);
  assert(instance->Get(sym::klass()) == klass);
}

TEST(Class_NewClassTest){
  BasicObjectPtr klass = BasicObject::New();
  Class::SetAsSubclass(klass, NULL);
  assert(klass->Get(sym::klass()) == Library::Instance()->Class());
  assert(klass->Get(sym::superclass()) == Library::Instance()->Object());
}

TEST(Class_SubClassTest){
  BasicObjectPtr klass = BasicObject::New();
  BasicObjectPtr subclass = BasicObject::New();
  Class::SetAsSubclass(subclass, klass);
  assert(subclass->Get(sym::klass()) == Library::Instance()->Class());
  assert(subclass->Get(sym::superclass()) == klass);
}

TEST(Class_InstanceMethodTest){
  BasicObjectPtr klass = BasicObject::New();
  BasicObjectPtr method = BasicObject::New();
  Class::SetAsInstanceMethod(klass, "AMethod", method);
  assert(klass->HasSlot(sym::instanceMethods()));
  // TODO: Test field named instance method has "AMethod".
}

TEST(Class_InitializeClassTest){
  BasicObjectPtr klass = Library::Instance()->Class();
  assert(klass.get());
  assert(klass->HasSlot(sym::klass()));
  assert(klass->Get(sym::klass()) == klass);
  assert(klass->HasSlot(sym::superclass()));
  assert(klass->Get(sym::superclass()) == Library::Instance()->Object());
}
