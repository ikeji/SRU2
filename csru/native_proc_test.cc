// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include "native_proc.h"

#include <cassert>
#include "library.h"
#include "stack_frame.h"
#include "interpreter.h"
#include "logging.h"

using namespace std;
using namespace sru;

DEFINE_SRU_PROC(SimpleMethod){
  return BasicObject::New();
}

TEST(NativeProc_CallTest){
  ptr_vector v;
  METHOD_SimpleMethod().Call(
      Library::Instance()->Nil(), CREATE_SRU_PROC(SimpleMethod), v);
  StackFrame* st = Interpreter::Instance()->CurrentStackFrame();
  assert(st);
  assert(st->ReturnValue().get());
}

DEFINE_SRU_PROC_SMASH(SimpleMethodSmash){
}

TEST(NativeProc_CallSmashTest){
  ptr_vector v;
  METHOD_SimpleMethodSmash().Call(
      Library::Instance()->Nil(), CREATE_SRU_PROC(SimpleMethodSmash),v);
}

DECLARE_SRU_PROC(SimpleMethod2);

TEST(NativeProc_DeclareTest){
  ptr_vector v;
  BasicObjectPtr proc = CREATE_SRU_PROC(SimpleMethod2);
  Proc* p = proc->GetData<Proc>();
  assert(p);
  p->Call(Library::Instance()->Nil(), proc, v);
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
  proc->Call(Library::Instance()->Nil(),obj,v);

  StackFrame* st = Interpreter::Instance()->CurrentStackFrame();
  assert(st);
  assert(st->ReturnValue() == Library::Instance()->String());
}

DEFINE_SRU_PROC(ReturnArgment){
  assert(args.size() > 0);
  return args[0];
}

TEST(NativeProc_PassArgTest){
  const BasicObjectPtr& obj = CREATE_SRU_PROC(ReturnArgment);
  assert(obj.get());
  Proc* proc = obj->GetData<Proc>();
  assert(proc);
  ptr_vector v;
  v.push_back(Library::Instance()->Numeric());
  proc->Call(Library::Instance()->Nil(),obj,v);

  StackFrame* st = Interpreter::Instance()->CurrentStackFrame();
  assert(st);
  assert(st->ReturnValue() == Library::Instance()->Numeric());
}

TEST(NativeProc_InspectTest){
  LOG_ERROR << CREATE_SRU_PROC(ReturnString)->Inspect();
  assert("<Proc({ -- Native Code -- })>" ==
         CREATE_SRU_PROC(ReturnString)->Inspect());
}
