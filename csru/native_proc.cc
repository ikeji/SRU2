// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "native_proc.h"

#include <string>
#include <iostream>
#include "interpreter.h"
#include "stack_frame.h"

using namespace sru;
using namespace std;

void NativeProc::Call(const ptr_vector& arg){
#ifdef DEBUG
  cout << "Call native proc with: ";
  for(ptr_vector::const_iterator it = arg.begin();
      it != arg.end();
      it++){
    cout << (*it)->Inspect() << " ";
  }
  cout << endl;
#endif
  BasicObjectPtr ret = method_body(arg);
  StackFrame* current_frame = Interpreter::Instance()->CurrentStackFrame();
  current_frame->PushResult(ret);
}
void NativeProcWithStackSmash::Call(const ptr_vector& arg){
  method_body_smash(arg);
}

string NativeProc::Inspect(){
  return "<Proc: { -- Native Code -- }>";
}
