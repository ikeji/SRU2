// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "proc.h"

#include <vector>
#include <string>
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
          const ptr_vector& expressions):
      varg(varg),
      retval(retval),
      expressions(Conv(expressions)){
  }
  void Call(const ptr_vector& arg);
 private:
  vector<string> varg;
  string retval;
  object_vector expressions;
};
}

BasicObjectPtr Proc::New(const std::vector<std::string>& varg,
             const std::string& retval,
             const ptr_vector& expressions){
  BasicObjectPtr obj = BasicObject::New(
      new SRUProc(varg,retval,expressions));
  Initialize(obj);
  return obj;
}

void SRUProc::Call(const ptr_vector& arg){
  BasicObjectPtr old_frame = StackFrame::New();
  StackFrame* frame = dynamic_cast<StackFrame*>(old_frame->Data());
  assert(frame);
  StackFrame* current_frame = dynamic_cast<StackFrame*>(
      Interpreter::Instance()->CurrentStackFrame()->Data());
  assert(current_frame);
  *frame = *current_frame;
  current_frame->SetUpStack(old_frame);
  current_frame->Setup(Conv(expressions));
  // TODO: bind varg to new frame.
}
