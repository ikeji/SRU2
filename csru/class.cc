// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "class.h"

#include <string>
#include "basic_object.h"
#include "constants.h"
#include "library.h"
#include "string.h"
#include "native_proc.h"
#include "interpreter.h"
#include "stack_frame.h"
#include "logging.h"
#include "symbol.h"

using namespace sru;
using namespace std;

void Class::InitializeInstance(const BasicObjectPtr& obj,
                               const BasicObjectPtr& klass){
  obj->Set(sym::klass(),klass);
  // TODO: InsertClassSystem
  LOG_TRACE << "Use findSlotMethod";
  obj->Set(sym::__findSlot(), klass->Get(sym::klass())->Get(sym::findSlotMethod()));
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

DEFINE_SRU_PROC_SMASH(findSlot){
  assert(args.size() >= 2);
  const BasicObjectPtr& obj = args[0];
  const symbol& name = SRUString::GetValue(args[1]);
  if(obj->HasSlot(sym::klass())){
    const BasicObjectPtr& klass = obj->Get(sym::klass());
    LOG << "Find in class." << klass->Inspect();
    if(klass->HasSlot(sym::instanceMethods())){
      const BasicObjectPtr& instance_slots = klass->Get(sym::instanceMethods());
      LOG << "Find in instance slot." << instance_slots->Inspect();
      if(instance_slots->HasSlot(name)){
        // Slot found.
        LOG << "Slot found: " << instance_slots->Get(name)->Inspect();
        Interpreter::Instance()->
          CurrentStackFrame()->
          PushResult(instance_slots->Get(name));
        return;
      }
    }
    LOG << "Not found";
    // TODO: Impliment inheritance.
    Interpreter::Instance()->
      CurrentStackFrame()->
      PushResult(Library::Instance()->Nil());
    return;
  }
  LOG << "Class instance not found";
  // This is not instance.
  Interpreter::Instance()->
  CurrentStackFrame()->
  PushResult(Library::Instance()->Nil());
}

DEFINE_SRU_PROC(subclass){
  assert(args.size() >= 1);
  BasicObjectPtr new_class = BasicObject::New();
  Class::SetAsSubclass(new_class, args[0]);
  if(args.size() == 2){
    new_class->Set(sym::__name(), args[1]);
  }
  return new_class;
}

DEFINE_SRU_PROC(object_new){
  assert(args.size() >= 1);
  BasicObjectPtr obj = BasicObject::New();
  Class::InitializeInstance(obj, args[0]);
  return obj;
}

void Class::InitializeClassClassFirst(const BasicObjectPtr& klass){
  klass->Set(sym::klass(), klass);
  BasicObjectPtr find_slot_method = BasicObject::New(new METHOD_findSlot());
  klass->Set(sym::findSlotMethod(), find_slot_method);
  LOG << "Setup findSlotMethod";
}

void Class::InitializeClassClassLast(const BasicObjectPtr& klass){
  Proc::Initialize(klass->Get(sym::findSlotMethod()));

  klass->Set(sym::__name(), SRUString::New(sym::Class()));
  SetAsSubclass(klass,NULL);

  SetAsInstanceMethod(klass, sym::subclass(), CREATE_SRU_PROC(subclass)); 
  SetAsInstanceMethod(klass, sym::mew(), CREATE_SRU_PROC(object_new));
}

