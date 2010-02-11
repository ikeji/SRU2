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

using namespace sru;
using namespace std;

DECLARE_SRU_PROC(FindSlot);

void Binding::InitializeClassObject(BasicObjectPtr binding){
  Class::InitializeInstance(binding, Library::Instance()->Class());
  binding->Set(fNAME, SRUString::New("Binding"));
  binding->Set("findSlotMethod", CREATE_SRU_PROC(FindSlot));
}

BasicObjectPtr Binding::New(const BasicObjectPtr& parent){
  BasicObjectPtr r = BasicObject::New();
  Class::InitializeInstance(r,Library::Instance()->Binding());
  if(parent.get() != NULL)
    r->Set(fPARENT_SCOPE, parent);
  r->Set(fFIND_SLOT, Library::Instance()->Binding()->Get("findSlotMethod"));
  return r;
}

DEFINE_SRU_PROC_SMASH(FindSlot){
  assert(args.size() >= 2);
  const BasicObjectPtr& env = args[0];
  const string& name = SRUString::GetValue(args[1]);
  if(!env->HasSlot(fPARENT_SCOPE)){
    Interpreter::Instance()->
      CurrentStackFrame()->
      PushResult(Library::Instance()->Nil());
    return;
  }
  const BasicObjectPtr& parent = env->Get(fPARENT_SCOPE);
  if(parent->HasSlot(name)){
    Interpreter::Instance()->
      CurrentStackFrame()->
      PushResult(parent->Get(name));
    return;
  }
  if(parent->HasSlot(fFIND_SLOT)){
    ptr_vector next_args;
    next_args.push_back(parent);
    next_args.push_back(args[1]);
    Proc::Invoke(parent->Get(fFIND_SLOT),next_args);
    return;
  }
  Interpreter::Instance()->
    CurrentStackFrame()->
    PushResult(Library::Instance()->Nil());
}
