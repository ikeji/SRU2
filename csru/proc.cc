// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "proc.h"

#include <vector>
#include <string>
#include <iostream>
#include "basic_object.h"
#include "object_vector.h"
#include "stack_frame.h"
#include "interpreter.h"
#include "library.h"

using namespace sru;
using namespace std;

namespace sru{

void Proc::Initialize(const BasicObjectPtr& obj){
  obj->Set("class", Library::Instance()->Proc());
}

class SRUProc : public Proc{
 public:
  SRUProc(const vector<string>& varg,
          const string retval,
          const ptr_vector& expressions,
          const BasicObjectPtr& binding):
      varg(varg),
      retval(retval),
      expressions(Conv(expressions)),
      binding(binding.get()){
  }
  void Call(const ptr_vector& arg);
  void Mark(){
    binding->Mark();
  }
 private:
  vector<string> varg;
  string retval;
  object_vector expressions;
  BasicObject* binding;

  SRUProc(const SRUProc& obj);
  SRUProc* operator=(const SRUProc& obj);
};
}

BasicObjectPtr Proc::New(const std::vector<std::string>& varg,
             const std::string& retval,
             const ptr_vector& expressions,
             const BasicObjectPtr& binding){
  BasicObjectPtr obj = BasicObject::New(
      new SRUProc(varg,retval,expressions,binding));
  Initialize(obj);
  return obj;
}

void SRUProc::Call(const ptr_vector& arg){
  BasicObjectPtr old_frame = StackFrame::New();
  StackFrame* frame = dynamic_cast<StackFrame*>(old_frame->Data());
#ifdef DEBUG
  cout << "Step in: " << frame << endl;
#endif
  assert(frame);
  StackFrame* current_frame = dynamic_cast<StackFrame*>(
      Interpreter::Instance()->CurrentStackFrame()->Data());
  assert(current_frame);
  *frame = *current_frame;
  current_frame->SetUpStack(old_frame);
  current_frame->Setup(Conv(expressions));
  for(unsigned int i=0;i<varg.size();i++){
    if(i<arg.size()){
      current_frame->Binding()->Set(varg[i],arg[i]);
    }else{
      current_frame->Binding()->Set(
          varg[i],Library::Instance()->Nil());
    }
  } 
}
