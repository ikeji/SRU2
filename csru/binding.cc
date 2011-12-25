// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "binding.h"

#include <string>
#include "basic_object.h"
#include "sru_string.h"
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
DECLARE_SRU_PROC(BindingGet);
DECLARE_SRU_PROC(BindingSet);

void Binding::InitializeClassObject(const BasicObjectPtr& binding){
  Class::InitializeInstance(binding, Library::Instance()->Class());
  binding->Set(sym::__name(), SRUString::New(sym::Binding()));
  binding->Set(sym::findSlotMethod(), CREATE_SRU_PROC(FindSlot));
  binding->Set(sym::getLocalMethod(), CREATE_SRU_PROC(BindingGet));
  binding->Set(sym::setLocalMethod(), CREATE_SRU_PROC(BindingSet));
}

BasicObjectPtr Binding::New(const BasicObjectPtr& parent){
  const BasicObjectPtr r = BasicObject::New();
  Class::InitializeInstance(r,Library::Instance()->Binding());
  r->Set(sym::local(), r);
  if(parent.get() != NULL)
    r->Set(sym::_parent(), parent);
  r->Set(sym::__findSlot(), Library::Instance()->Binding()->Get(sym::findSlotMethod()));
  r->Set(sym::get(), Library::Instance()->Binding()->Get(sym::getLocalMethod()));
  r->Set(sym::set(), Library::Instance()->Binding()->Get(sym::setLocalMethod()));
  return r;
}

DEFINE_SRU_PROC_SMASH(FindSlot){
  ARGLEN(2);
  const BasicObjectPtr& env = args[0];
  LOG_TRACE << env->Inspect();
  const symbol& name = SRUString::GetValue(args[1]);
  if(!env->HasSlot(sym::_parent())){
    LOG << "Parent not found.";
    PushResult(Library::Instance()->Nil());
    return;
  }
  const BasicObjectPtr& parent = env->Get(sym::_parent());
  if(parent->HasSlot(name)){
    LOG << "find in parent: " << parent->Inspect() << " --> " << parent->Get(name)->Inspect();
    PushResult(parent->Get(name));
    return;
  }
  if(parent->HasSlot(sym::__findSlot())){
    ptr_vector next_args;
    next_args.push_back(parent);
    next_args.push_back(args[1]);
    Proc::Invoke(context, parent->Get(sym::__findSlot()), next_args);
    return;
  }
  LOG << "Not found.";
  PushResult(Library::Instance()->Nil());
}

DEFINE_SRU_PROC(BindingGet){
  ARGLEN(2);
  const BasicObjectPtr& env = args[0];
  LOG_TRACE << env->Inspect();
  const symbol& name = SRUString::GetValue(args[1]);
  if(!env->HasSlot(name)){
    return Library::Instance()->Nil();
  }
  return env->Get(name);
}

DEFINE_SRU_PROC(BindingSet){
  ARGLEN(3);
  const BasicObjectPtr& env = args[0];
  LOG_TRACE << env->Inspect();
  const symbol& name = SRUString::GetValue(args[1]);
  env->Set(name, args[2]);
  return args[2];
}
