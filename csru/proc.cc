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

DECLARE_SRU_PROC(whileTrue);
DECLARE_SRU_PROC(loop);

void Proc::Initialize(const BasicObjectPtr& obj){
  Class::InitializeInstance(obj,Library::Instance()->Proc());

  // HACK: avoid infinite loop.
  static BasicObjectPtr while_true_instance;
  if(while_true_instance.get() == NULL) {
    while_true_instance = BasicObject::New(whileTrue);
    Initialize(while_true_instance);
  }
  static BasicObjectPtr loop_instance;
  if(loop_instance.get() == NULL) {
    loop_instance = BasicObject::New(loop);
    Initialize(loop_instance);
  }
  // TODO: We need class system.
  // TODO: test this functions existed or not.
  obj->Set("whileTrue", while_true_instance); 
  obj->Set("loop", loop_instance); 
}

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
  SRUProc(const vector<string>& vargs,
          const string retval,
          const ptr_vector& expressions,
          const BasicObjectPtr& binding):
      vargs(vargs),
      retval(retval),
      // TODO: Check expressions are instance of AST.
      expressions(Conv(expressions)),
      binding(binding.get()){
  }
  void Call(const BasicObjectPtr& proc, const ptr_vector& args);
  void Mark(){
    binding->Mark();
    MarkVector(&expressions);
  }
  string Inspect();
 private:
  vector<string> vargs;
  string retval;
  object_vector expressions;
  BasicObject* binding;

  SRUProc(const SRUProc& obj);
  SRUProc* operator=(const SRUProc& obj);
};

BasicObjectPtr Proc::New(const std::vector<std::string>& vargs,
             const std::string& retval,
             const ptr_vector& expressions,
             const BasicObjectPtr& binding){
  BasicObjectPtr obj = BasicObject::New(
      new SRUProc(vargs,retval,expressions,binding));
  Initialize(obj);
  return obj;
}

string SRUProc::Inspect(){
  string ret = "";
  for(vector<string>::iterator it = vargs.begin();
      it != vargs.end();
      it++){
    if(it != vargs.begin())
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

void SRUProc::Call(const BasicObjectPtr& proc, const ptr_vector& args){
#ifdef DEBUG
      cout << "Call sru function" << endl;
#endif
  BasicObjectPtr new_binding = Binding::New(binding);
  Interpreter::Instance()->DigIntoNewFrame(Conv(expressions),new_binding);
  for(unsigned int i=0;i<vargs.size();i++){
    if(i<args.size()){
#ifdef DEBUG
      cout << "Bind-arg: " << vargs[i] << " = " << args[i]->Inspect() << endl;
#endif
      new_binding->Set(vargs[i],args[i]);
    }else{
#ifdef DEBUG
      cout << "Bind-arg: " << vargs[i] << " = Nil" << endl;
#endif
      new_binding->Set(vargs[i],Library::Instance()->Nil());
    }
  }
  if(!retval.empty())
    new_binding->Set(retval,Interpreter::Instance()->GetContinationToEscapeFromCurrentStack());
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

// TODO: test this function.
DEFINE_SRU_PROC_SMASH(whileTrue){
  static BasicObjectPtr whileTrue_internal;
  assert(args.size() > 1);
  BasicObjectPtr new_binding = Binding::New();
  Interpreter::Instance()->DigIntoNewFrame(
      A(C(R("whileTrue_internal"))),
      new_binding);
  // TODO: Muliti thread support.
  if(whileTrue_internal == NULL)
    whileTrue_internal = CREATE_SRU_PROC(_whileTrue_internal);

  new_binding->Set("whileTrue_internal",whileTrue_internal);
  new_binding->Set("condition", args[0]);
  new_binding->Set("block", args[1]);
}

DEFINE_SRU_PROC_SMASH(_loop_internal){
  Interpreter::Instance()->CurrentStackFrame()->Setup(
      A(
        C(R("block")),
        C(R("block"),R("loop"))
       ));
}

// TODO: test this function.
DEFINE_SRU_PROC_SMASH(loop){
  static BasicObjectPtr loop_internal;
  assert(args.size() > 0);
  BasicObjectPtr new_binding = Binding::New();
  Interpreter::Instance()->DigIntoNewFrame(
      A(C(R("loop_internal"))),
      new_binding);
  // TODO: Muliti thread support.
  if(loop_internal == NULL)
    loop_internal = CREATE_SRU_PROC(_loop_internal);

  new_binding->Set("loop_internal",loop_internal);
  new_binding->Set("block", args[0]);
}

}  // namespace sru
