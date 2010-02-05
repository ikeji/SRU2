// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include "native_proc.h"

#include <iostream>
#include <cassert>
#include "library.h"
#include "stack_frame.h"
#include "interpreter.h"

using namespace std;
using namespace sru;

DEFINE_SRU_PROC(SimpleMethod){
  return BasicObject::New();
}

TEST(NativeProc_CallTest){
  ptr_vector v;
  SimpleMethod->Call(CREATE_SRU_PROC(SimpleMethod), v);
  StackFrame* st = Interpreter::Instance()->CurrentStackFrame();
  assert(st);
  assert(st->ReturnValue().get());
}

DEFINE_SRU_PROC_SMASH(SimpleMethodSmash){
}

TEST(NativeProc_CallSmashTest){
  ptr_vector v;
  SimpleMethodSmash->Call(CREATE_SRU_PROC(SimpleMethodSmash),v);
}

DECLARE_SRU_PROC(SimpleMethod2);

TEST(NativeProc_DeclareTest){
  ptr_vector v;
  SimpleMethod2->Call(CREATE_SRU_PROC(SimpleMethod2),v);
  StackFrame* st = Interpreter::Instance()->CurrentStackFrame();
  assert(st);
  assert(st->ReturnValue().get());
}

DEFINE_SRU_PROC(SimpleMethod2){
  return BasicObject::New();
}

DEFINE_SRU_PROC(ReturnString){
  return Library::Instance()->String();
}

TEST(NativeProc_EvalTest){
  const BasicObjectPtr& obj = CREATE_SRU_PROC(ReturnString);
  assert(obj.get());
  Proc* proc = obj->GetData<Proc>();
  assert(proc);
  ptr_vector v;
  proc->Call(obj,v);

  StackFrame* st = Interpreter::Instance()->CurrentStackFrame();
  assert(st);
  assert(st->ReturnValue() == Library::Instance()->String());
}

DEFINE_SRU_PROC(ReturnArgment){
  assert(arg.size() > 0);
  return arg[0];
}

TEST(NativeProc_PassArgTest){
  const BasicObjectPtr& obj = CREATE_SRU_PROC(ReturnArgment);
  assert(obj.get());
  Proc* proc = obj->GetData<Proc>();
  assert(proc);
  ptr_vector v;
  v.push_back(Library::Instance()->Numeric());
  proc->Call(obj,v);

  StackFrame* st = Interpreter::Instance()->CurrentStackFrame();
  assert(st);
  assert(st->ReturnValue() == Library::Instance()->Numeric());
}

TEST(NativeProc_InspectTest){
  cout << CREATE_SRU_PROC(ReturnString)->Inspect() << endl;
  assert("<Proc: { -- Native Code -- }>" ==
         CREATE_SRU_PROC(ReturnString)->Inspect());
}
