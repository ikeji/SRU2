// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "class.h"

#include <string>
#include "binding.h"
#include "basic_object.h"
#include "constants.h"
#include "library.h"
#include "sru_string.h"
#include "native_proc.h"
#include "interpreter.h"
#include "stack_frame.h"
#include "logging.h"
#include "symbol.h"

// TODO: remove this dependency
#include "testing_ast.h"
#include "testing_sru.h"
using namespace sru_test;

using namespace sru;
using namespace std;

void Class::InitializeInstance(const BasicObjectPtr& obj,
                               const BasicObjectPtr& klass){
  obj->Set(sym::klass(),klass);
  // TODO: InsertClassSystem
  LOG_TRACE << "Use findSlotMethod";
  obj->Set(sym::__findSlot(), klass->Get(sym::klass())->Get(sym::findSlotMethod()));
  obj->Set(sym::findInstanceMethod(), klass->Get(sym::klass())->Get(sym::findInstanceMethod()));
}

void Class::SetAsSubclass(const BasicObjectPtr& obj,
                          const BasicObjectPtr& super_klass){
  BasicObjectPtr klass = super_klass;
  if(klass.get() == NULL) klass = Library::Instance()->Object();
  InitializeInstance(obj, Library::Instance()->Class());
  obj->Set(sym::superclass(), klass);
  obj->Set(sym::instanceMethods(), BasicObject::New());
}

void Class::SetAsInstanceMethod(const BasicObjectPtr& klass,
                                const symbol& name,
                                const BasicObjectPtr& method){
  if(! klass->HasSlot(sym::instanceMethods()))
    // TODO: Use Map class here!
    klass->Set(sym::instanceMethods(),BasicObject::New());
  klass->Get(sym::instanceMethods())->Set(name, method);
}

DEFINE_SRU_PROC_SMASH(findInstanceMethod){
  ARGLEN(2);
  const BasicObjectPtr& klass = args[0];
  const symbol& name = SRUString::GetValue(args[1]);
  if(! klass->HasSlot(sym::instanceMethods())){
    LOG << "Instance methods not found";
    // TODO: Impliment inheritance.
    PushResult(Library::Instance()->Nil());
    return;
  }
  const BasicObjectPtr& instance_slots = klass->Get(sym::instanceMethods());
  LOG << "Find in instance slot." << instance_slots->Inspect();
  if(instance_slots->HasSlot(name)){
    // Slot found.
    LOG << "Slot found: " << instance_slots->Get(name)->Inspect();
    PushResult(instance_slots->Get(name));
    return;
  }
  // TODO: Find in super class.
  PushResult(Library::Instance()->Nil());
  return;
}

DEFINE_SRU_PROC_SMASH(findSlot){
  ARGLEN(2);
  const BasicObjectPtr& obj = args[0];
  const symbol& name = SRUString::GetValue(args[1]);
  LOG << "Find Slot named " << name.to_str() <<
    " in " << obj->Inspect();
  if(! obj->HasSlot(sym::klass())){
    LOG << "Class instance not found";
    // This is not instance.
    PushResult(Library::Instance()->Nil());
    return;
  }
  const BasicObjectPtr& klass = obj->Get(sym::klass());
  LOG << "Find in class." << klass->Inspect();
  const BasicObjectPtr new_binding = Binding::New();
  Interpreter::Instance()->DigIntoNewFrame(
      A(C(R(R(sym::self()),sym::findInstanceMethod()),
          R(sym::self()),
          R(sym::__name()))),
      new_binding);
  new_binding->Set(sym::self(), klass);
  new_binding->Set(sym::__name(), args[1]);
}

DEFINE_SRU_PROC(subclass){
  ARGLEN(1);
  const BasicObjectPtr new_class = BasicObject::New();
  Class::SetAsSubclass(new_class, args[0]);
  if(args.size() >= 2){
    new_class->Set(sym::__name(), args[1]);
  }
  return new_class;
}

DEFINE_SRU_PROC(object_new){
  ARGLEN(1);
  const BasicObjectPtr obj = BasicObject::New();
  Class::InitializeInstance(obj, args[0]);
  return obj;
}

void Class::InitializeClassClassFirst(const BasicObjectPtr& klass){
  klass->Set(sym::klass(), klass);
  const BasicObjectPtr find_slot_method = BasicObject::New(new METHOD_findSlot());
  const BasicObjectPtr find_instance_method =
    BasicObject::New(new METHOD_findInstanceMethod());
  klass->Set(sym::findSlotMethod(), find_slot_method);
  klass->Set(sym::findInstanceMethod(), find_instance_method);
  LOG << "Setup findSlotMethod";
}

void Class::InitializeClassClassLast(const BasicObjectPtr& klass){
  Proc::Initialize(klass->Get(sym::findSlotMethod()));
  Proc::Initialize(klass->Get(sym::findInstanceMethod()));

  klass->Set(sym::__name(), SRUString::New(sym::Class()));
  SetAsSubclass(klass,NULL);

  SetAsInstanceMethod(klass, sym::subclass(), CREATE_SRU_PROC(subclass)); 
  SetAsInstanceMethod(klass, sym::mew(), CREATE_SRU_PROC(object_new));
}

