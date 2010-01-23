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

TEST(NativeProc_Call){
  ptr_vector v;
  SimpleMethod.Call(v);
}

DEFINE_SRU_PROC(ReturnString){
  return Library::Instance()->String();
}

TEST(NativeProc_Eval){
  const BasicObjectPtr& obj = ReturnString.New();
  assert(obj.get());
  Proc* proc = dynamic_cast<Proc*>(obj->Data());
  assert(proc);
  ptr_vector v;
  proc->Call(v);

  StackFrame* st = dynamic_cast<StackFrame*>(
      Interpreter::Instance()->CurrentStackFrame()->Data());
  assert(st);
  assert(st->ReturnValue() == Library::Instance()->String());
}

TEST(NativeProc_Inspect){
  cout << ReturnString.New()->Inspect() << endl;
  assert("<Proc: { -- Native Code -- }>" == ReturnString.New()->Inspect());
}
