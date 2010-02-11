// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "class.h"

#include <string>
#include <iostream>
#include "basic_object.h"
#include "constants.h"
#include "library.h"
#include "string.h"
#include "native_proc.h"
#include "interpreter.h"
#include "stack_frame.h"

using namespace sru;
using namespace std;

void Class::InitializeInstance(const BasicObjectPtr& obj,
                               const BasicObjectPtr& klass){
  obj->Set(fCLASS,klass);
  // TODO: InsertClassSystem
#ifdef DEBUG2
  cout << "Use findSlotMethod" << endl;
#endif
  obj->Set(fFIND_SLOT, klass->Get(fCLASS)->Get("findSlotMethod"));
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

DECLARE_SRU_PROC(findSlot);

void Class::InitializeClassClassFirst(const BasicObjectPtr& klass){
  klass->Set(fCLASS, klass);
  BasicObjectPtr find_slot_method = BasicObject::New(findSlot);
  klass->Set("findSlotMethod", find_slot_method);
#ifdef DEBUG2
  cout << "Setup findSlotMethod" << endl;
#endif
}

void Class::InitializeClassClassLast(const BasicObjectPtr& klass){
  Proc::Initialize(klass->Get("findSlotMethod"));

  klass->Set(fNAME, SRUString::New("Class"));
  SetAsSubclass(klass,NULL);
}

DEFINE_SRU_PROC_SMASH(findSlot){
  assert(args.size() >= 2);
  const BasicObjectPtr& obj = args[0];
  const string& name = SRUString::GetValue(args[1]);
  if(obj->HasSlot(fCLASS)){
    const BasicObjectPtr& klass = obj->Get(fCLASS);
    if(klass->HasSlot(fINSTANCE_METHODS)){
      const BasicObjectPtr& instance_slots = klass->Get(fINSTANCE_METHODS);
      if(instance_slots->HasSlot(name)){
        // Slot found.
        Interpreter::Instance()->
          CurrentStackFrame()->
          PushResult(instance_slots->Get(name));
        return;
      }
    }
    // TODO: Impliment inheritance.
    Interpreter::Instance()->
      CurrentStackFrame()->
      PushResult(Library::Instance()->Nil());
    return;
  }
  // This is not instance.
  Interpreter::Instance()->
  CurrentStackFrame()->
  PushResult(Library::Instance()->Nil());
}
