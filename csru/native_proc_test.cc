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
  SimpleMethod->Call(v);
  StackFrame* st = Interpreter::Instance()->CurrentStackFrame();
  assert(st);
  assert(st->ReturnValue().get());
}

DECLARE_SRU_PROC(SimpleMethod2);

TEST(NativeProc_DeclareTest){
  ptr_vector v;
  SimpleMethod2->Call(v);
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
  proc->Call(v);

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
  proc->Call(v);

  StackFrame* st = Interpreter::Instance()->CurrentStackFrame();
  assert(st);
  assert(st->ReturnValue() == Library::Instance()->Numeric());
}

TEST(NativeProc_InspectTest){
  cout << CREATE_SRU_PROC(ReturnString)->Inspect() << endl;
  assert("<Proc: { -- Native Code -- }>" ==
         CREATE_SRU_PROC(ReturnString)->Inspect());
}
