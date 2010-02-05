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
#include "constants.h"
#include "class.h"
#include "native_proc.h"
#include "string.h"
#include "class.h"

// TODO: remove this dependency
#include "testing_ast.h"
#include "testing_sru.h"
using namespace sru_test;

using namespace sru;
using namespace std;

namespace sru{

void Proc::Initialize(const BasicObjectPtr& obj){
  Class::InitializeInstance(obj,Library::Instance()->Proc());
}

DECLARE_SRU_PROC(whileTrue);

void Proc::InitializeClassObject(const BasicObjectPtr& proc){
  Class::SetAsSubclass(proc, NULL);
  proc->Set(fNAME, SRUString::New("Proc"));
  Class::SetAsInstanceMethod(proc, "whileTrue", CREATE_SRU_PROC(whileTrue));
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
    ret += (*it)->GetData<Expression>()->InspectAST() + ";";
  }
  return ret + ">";
}

void SRUProc::Call(const ptr_vector& arg){
#ifdef DEBUG
      cout << "Call sru function" << endl;
#endif
  BasicObjectPtr new_binding = Binding::New(binding);
  Interpreter::Instance()->DigIntoNewFrame(Conv(expressions),new_binding);
  for(unsigned int i=0;i<varg.size();i++){
    if(i<arg.size()){
#ifdef DEBUG
      cout << "Bind-arg: " << varg[i] << " = " << arg[i]->Inspect() << endl;
#endif
      new_binding->Set(varg[i],arg[i]);
    }else{
#ifdef DEBUG
      cout << "Bind-arg: " << varg[i] << " = Nil" << endl;
#endif
      new_binding->Set(varg[i],Library::Instance()->Nil());
    }
  }
#ifdef DEBUG
  cout << "Current-Binding: " << new_binding->Inspect() << endl;
#endif
}

DEFINE_SRU_PROC_SMASH(_whileTrue_internal){
  Interpreter::Instance()->CurrentStackFrame()->Setup(
      A(
        L("result", C(R("condition"))),  // result = condition()
        C(
          R(R("result"),"ifTrue"),       // if(result){
          P(C(R("block")),               //   block();
            C(R("whileTrue_internal"))   //   whileTrue_internal();
          )                              // }
        ))
      );
}

DEFINE_SRU_PROC_SMASH(whileTrue){
  static BasicObjectPtr whileTrue_internal;
  assert(arg.size() > 1);
  BasicObjectPtr new_binding = Binding::New();
  Interpreter::Instance()->DigIntoNewFrame(
      A(C(R("whileTrue_internal"))),
      new_binding);
  if(whileTrue_internal == NULL)
    whileTrue_internal = CREATE_SRU_PROC(_whileTrue_internal);

  new_binding->Set("whileTrue_internal",whileTrue_internal);
  new_binding->Set("condition", arg[0]);
  new_binding->Set("block", arg[1]);
}

}  // namespace sru
