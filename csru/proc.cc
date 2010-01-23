// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "proc.h"

#include <vector>
#include <string>
#include <iostream>
#include "ast.h"
#include "basic_object.h"
#include "binding.h"
#include "interpreter.h"
#include "library.h"
#include "object_vector.h"
#include "stack_frame.h"

using namespace sru;
using namespace std;

namespace sru{

void Proc::Initialize(const BasicObjectPtr& obj){
  obj->Set("class", Library::Instance()->Proc());
}

string Proc::Inspect(){
  return "<Proc>";
}

class SRUProc : public Proc{
 public:
  SRUProc(const vector<string>& varg,
          const string retval,
          const ptr_vector& expressions,
          const BasicObjectPtr& binding):
      varg(varg),
      retval(retval),
      // TODO: Check expressions are instance of AST.
      expressions(Conv(expressions)),
      binding(binding.get()){
  }
  void Call(const ptr_vector& arg);
  void Mark(){
    binding->Mark();
    MarkVector(&expressions);
  }
  string Inspect();
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

string SRUProc::Inspect(){
  string ret = "";
  for(vector<string>::iterator it = varg.begin();
      it != varg.end();
      it++){
    if(it != varg.begin())
      ret += ",";
    ret += *it;
  }
  if(!retval.empty()) ret += ":" + retval;
  if(!ret.empty()) ret = "|" + ret + "|";
  ret = "<Proc: {";
  for(object_vector::const_iterator it = expressions.begin();
      it != expressions.end();
      it++){
    ret += dynamic_cast<Expression*>((*it)->Data())->InspectAST() + ";";
  }
  return ret + ">";
}

void SRUProc::Call(const ptr_vector& arg){
  BasicObjectPtr old_frame = StackFrame::New();
  StackFrame* frame = dynamic_cast<StackFrame*>(old_frame->Data());
#ifdef DEBUG
  cout << "Step in: " << old_frame->Inspect() << endl;
#endif
  assert(frame);
  StackFrame* current_frame = dynamic_cast<StackFrame*>(
      Interpreter::Instance()->CurrentStackFrame()->Data());
  assert(current_frame);
  *frame = *current_frame;
  current_frame->SetBinding(Binding::New(binding));
  current_frame->SetUpperStack(old_frame);
  current_frame->Setup(Conv(expressions));
  for(unsigned int i=0;i<varg.size();i++){
    if(i<arg.size()){
#ifdef DEBUG
      cout << "Bind-arg: " << varg[i] << " = " << arg[i]->Inspect() << endl;
#endif
      current_frame->Binding()->Set(varg[i],arg[i]);
    }else{
#ifdef DEBUG
      cout << "Bind-arg: " << varg[i] << " = Nil" << endl;
#endif
      current_frame->Binding()->Set(
          varg[i],Library::Instance()->Nil());
    }
  }
#ifdef DEBUG
  cout << "Currend-Binding: " << current_frame->Binding()->Inspect() << endl;
#endif
}
