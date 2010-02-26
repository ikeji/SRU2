// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "binding.h"

#include <string>
#include "basic_object.h"
#include "string.h"
#include "class.h"
#include "library.h"
#include "constants.h"
#include "native_proc.h"
#include "interpreter.h"
#include "stack_frame.h"
#include "symbol.h"

using namespace sru;
using namespace std;

DECLARE_SRU_PROC(FindSlot);

void Binding::InitializeClassObject(BasicObjectPtr binding){
  Class::InitializeInstance(binding, Library::Instance()->Class());
  binding->Set(sym::name(), SRUString::New(sym::Binding()));
  binding->Set(sym::findSlotMethod(), CREATE_SRU_PROC(FindSlot));
}

BasicObjectPtr Binding::New(const BasicObjectPtr& parent){
  BasicObjectPtr r = BasicObject::New();
  Class::InitializeInstance(r,Library::Instance()->Binding());
  if(parent.get() != NULL)
    r->Set(sym::parent(), parent);
  r->Set(sym::findSlot(), Library::Instance()->Binding()->Get(sym::findSlotMethod()));
  return r;
}

DEFINE_SRU_PROC_SMASH(FindSlot){
  assert(args.size() >= 2);
  const BasicObjectPtr& env = args[0];
  LOG_TRACE << env->Inspect();
  const symbol& name = SRUString::GetValue(args[1]);
  if(!env->HasSlot(sym::parent())){
    LOG << "Parent not found.";
    Interpreter::Instance()->
      CurrentStackFrame()->
      PushResult(Library::Instance()->Nil());
    return;
  }
  const BasicObjectPtr& parent = env->Get(sym::parent());
  if(parent->HasSlot(name)){
    LOG << "find in parent: " << parent->Inspect() << " --> " << parent->Get(name)->Inspect();
    Interpreter::Instance()->
      CurrentStackFrame()->
      PushResult(parent->Get(name));
    return;
  }
  if(parent->HasSlot(sym::findSlot())){
    ptr_vector next_args;
    next_args.push_back(parent);
    next_args.push_back(args[1]);
    Proc::Invoke(parent->Get(sym::findSlot()),next_args);
    return;
  }
  LOG << "Not found.";
  Interpreter::Instance()->
    CurrentStackFrame()->
    PushResult(Library::Instance()->Nil());
}
