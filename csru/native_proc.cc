// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "native_proc.h"

#include <string>
#include "interpreter.h"
#include "stack_frame.h"

using namespace sru;
using namespace std;

void NativeProc::Call(const ptr_vector& arg){
  BasicObjectPtr ret = method_body(arg);
  StackFrame* current_frame = dynamic_cast<StackFrame*>(
      Interpreter::Instance()->CurrentStackFrame()->Data());
  assert(current_frame);
  current_frame->PushResult(ret);
}

string NativeProc::Inspect(){
  return "<Proc: { -- Native Code -- }>";
}
