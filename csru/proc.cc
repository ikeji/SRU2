// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "proc.h"

#include <vector>
#include <string>
#include "ast.h"
#include "basic_object.h"
#include "binding.h"
#include "interpreter.h"
#include "library.h"
#include "object_container.h"
#include "stack_frame.h"
#include "constants.h"
#include "class.h"
#include "native_proc.h"
#include "sru_string.h"
#include "numeric.h"
#include "class.h"
#include "logging.h"
#include "symbol.h"
#include "utils.h"
#include "sru_array.h"

// TODO: remove this dependency
#include "testing_ast.h"
#include "testing_sru.h"
using namespace sru_test;

using namespace sru;
using namespace std;

namespace sru{

void Proc::Initialize(const BasicObjectPtr& obj){
  Class::InitializeInstance(obj,Library::Instance()->Proc());
  // TODO: test this functions existed or not.
}

DECLARE_SRU_PROC(whileTrue);
DECLARE_SRU_PROC(loop);
DECLARE_SRU_PROC(Apply);
DECLARE_SRU_PROC(Ylppa);

void Proc::InitializeClassObject(const BasicObjectPtr& proc){
  Class::SetAsSubclass(proc, NULL);
  proc->Set(sym::__name(), SRUString::New(sym::Proc()));
  Class::SetAsInstanceMethod(proc, sym::whileTrue(), CREATE_SRU_PROC(whileTrue));
  Class::SetAsInstanceMethod(proc, sym::loop(), CREATE_SRU_PROC(loop));
  Class::SetAsInstanceMethod(proc, sym::apply(), CREATE_SRU_PROC(Apply));
  Class::SetAsInstanceMethod(proc, sym::ylppa(), CREATE_SRU_PROC(Ylppa));
}

string Proc::Inspect(){
  return "Proc";
}

void PrintErrorContext(const BasicObjectPtr& context, const ptr_vector args){
  CallExpression* call = context->GetData<CallExpression>();
  if(call && !IsNil(call->Proc())){
    RefExpression* ref = call->Proc()->GetData<RefExpression>();
    if(ref && !IsNil(ref->Name())) {
      if(IsNil(ref->Env())){
        LOG_ALWAYS << "At call " << ref->Name()->Inspect();
      }else{
        LOG_ALWAYS << "At call method " << ref->Name()->Inspect()
          << " for " << ref->Env()->Inspect();
      }
    }
    if(args.size() > 1){
      LOG_ALWAYS << "With " << args[0]->Inspect() << ", ... ";
    } else {
      LOG_ALWAYS << "With " << args[0]->Inspect();
    }
  }
  // Abort if context not found.
  if (!(context->HasSlot(sym::src()) && 
        context->HasSlot(sym::pos()))) return;
  const string& src = SRUString::GetValue(context->Get(sym::src())).to_str();
  const size_t pos = SRUNumeric::GetIntValue(context->Get(sym::pos()));
  PrintErrorPosition(src, pos);
}

void Proc::Invoke(const BasicObjectPtr& context,
                  const BasicObjectPtr& proc,
                  const ptr_vector& args){
  // TODO: Show more meaningful error.
  Proc* p = proc->GetData<Proc>();
  if (!p) {
    PrintErrorContext(context, args);
  }
  DCHECK(p) << "Can't invoke " << proc->Inspect() << " object";
  p->Call(context, proc, args);
}

void Proc::Dispose(){
  delete this;
}

class SRUProc : public Proc{
 public:
  SRUProc(const vector<symbol>& vargs,
          const symbol retval,
          const ptr_vector& expressions,
          const BasicObjectPtr& binding):
      vargs(vargs),
      retval(retval),
      // TODO: Check expressions are instance of AST.
      expressions(Conv(expressions)),
      binding(binding.get()){
  }
  void Call(const BasicObjectPtr& context,
            const BasicObjectPtr& proc,
            const ptr_vector& args);
  void Mark(){
    binding->Mark();
    MarkVector(&expressions);
  }
  string Inspect();
 private:
  vector<symbol> vargs;
  symbol retval;
  object_vector expressions;
  BasicObject* binding;

  SRUProc(const SRUProc& obj);
  SRUProc* operator=(const SRUProc& obj);
};

BasicObjectPtr Proc::New(const std::vector<symbol>& vargs,
             const symbol& retval,
             const ptr_vector& expressions,
             const BasicObjectPtr& binding){
  const BasicObjectPtr obj = BasicObject::New(
      new SRUProc(vargs,retval,expressions,binding));
  Initialize(obj);
  return obj;
}

string SRUProc::Inspect(){
  string ret = "";
  for(vector<symbol>::const_iterator it = vargs.begin();
      it != vargs.end();
      it++){
    if(it != vargs.begin())
      ret += ",";
    ret += it->to_str();
  }
  if(ret == "_") ret = "";  // skip default argument.
  if(!retval.to_str().empty()) ret += ":" + retval.to_str();
  if(!ret.empty()) ret = "|" + ret + "|";
  ret = string("Proc({") + ret;
  for(object_vector::const_iterator it = expressions.begin();
      it != expressions.end();
      it++){
    Expression* exp = (*it)->GetData<Expression>();
    assert(exp);
    ret += exp->InspectAST() + ";";
  }
  return ret + "})";
}

void SRUProc::Call(const BasicObjectPtr& context,
                   const BasicObjectPtr& proc,
                   const ptr_vector& args){
  LOG << "Call sru function";
  const BasicObjectPtr new_binding = Binding::New(binding);
  Interpreter::Instance()->DigIntoNewFrame(Conv(expressions),new_binding);
  for(unsigned int i=0;i<vargs.size();i++){
    if(i<args.size()){
      LOG << "Bind-arg: " << vargs[i].to_str() << " = " << args[i]->Inspect();
      new_binding->Set(vargs[i],args[i]);
    }else{
      LOG << "Bind-arg: " << vargs[i].to_str() << " = Nil";
      new_binding->Set(vargs[i],Library::Instance()->Nil());
    }
  }
  if(!retval.to_str().empty())
    new_binding->Set(retval,Interpreter::Instance()->GetContinationToEscapeFromCurrentStack());
  LOG << "Current-Binding: " << new_binding->Inspect();
}

DEFINE_SRU_PROC_SMASH(_whileTrue_internal){
  Interpreter::Instance()->CurrentStackFrame()->Setup(
      A(
        L(sym::result(), C(R(sym::condition()))),  // result = condition()
        C(
          R(R(sym::result()),sym::ifTrue()),       // if(result){
          R(sym::result()),
          P(C(R(sym::block())),                    //   block();
            C(R(sym::whileTrue_internal()))        //   whileTrue_internal();
          )                                        // }
        ))
      );
}

// TODO: test this function.
DEFINE_SRU_PROC_SMASH(whileTrue){
  static BasicObjectPtr whileTrue_internal;
  ARGLEN(2);
  const BasicObjectPtr new_binding = Binding::New();
  Interpreter::Instance()->DigIntoNewFrame(
      A(C(R(sym::whileTrue_internal()))),
      new_binding);
  // TODO: Muliti thread support.
  if(whileTrue_internal == NULL)
    whileTrue_internal = CREATE_SRU_PROC(_whileTrue_internal);

  new_binding->Set(sym::whileTrue_internal(),whileTrue_internal);
  new_binding->Set(sym::condition(), args[0]);
  new_binding->Set(sym::block(), args[1]);
}

DEFINE_SRU_PROC_SMASH(_loop_internal){
  Interpreter::Instance()->CurrentStackFrame()->Setup(
      A(
        C(R(sym::block())),
        C(R(sym::loop_internal()))
       ));
}

// TODO: test this function.
DEFINE_SRU_PROC_SMASH(loop){
  static BasicObjectPtr loop_internal;
  ARGLEN(1);
  const BasicObjectPtr new_binding = Binding::New();
  Interpreter::Instance()->DigIntoNewFrame(
      A(C(R(sym::loop_internal()))),
      new_binding);
  // TODO: Muliti thread support.
  if(loop_internal == NULL)
    loop_internal = CREATE_SRU_PROC(_loop_internal);

  new_binding->Set(sym::loop_internal(),loop_internal);
  new_binding->Set(sym::block(), args[0]);
}

DEFINE_SRU_PROC_SMASH(Apply){
  ARGLEN(2);
  Array* array = args[1]->GetData<Array>();
  DCHECK(array) << "Apply requires array.";
  Proc::Invoke(context, args[0], Conv(*(array->GetValue())));
}

DEFINE_SRU_PROC_SMASH(Ylppa_internal){
  BasicObjectPtr a = Array::New();
  Array* array = a->GetData<Array>();
  for(ptr_vector::const_iterator it = args.begin();
      it != args.end();
      it++){
    array->GetValue()->push_back((*it).get());
  }

  DCHECK(proc->HasSlot(sym::proc()));
  BasicObjectPtr main = proc->Get(sym::proc());

  ptr_vector arg;
  arg.push_back(a);

  Proc::Invoke(context,main,arg);
}

DEFINE_SRU_PROC(Ylppa){
  ARGLEN(1);
  const BasicObjectPtr ret = CREATE_SRU_PROC(Ylppa_internal);
  ret->Set(sym::proc(), args[0]);
  return ret;
}

}  // namespace sru
