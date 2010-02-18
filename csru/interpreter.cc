// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "interpreter.h"

#include <vector>
#include "ast.h"
#include "basic_object.h"
#include "object_vector.h"
#include "stack_frame.h"
#include "library.h"
#include "binding.h"
#include "native_proc.h"
#include "logging.h"

using namespace sru;
using namespace std;

struct Interpreter::Impl {
  Impl(): current_frame(), root_frame(){
  }
  BasicObjectPtr current_frame;
  BasicObjectPtr root_frame;
};

Interpreter::Interpreter():pimpl(new Impl()){
  InitializeInterpreter();
}

Interpreter::~Interpreter(){
  delete pimpl;
}

void Interpreter::InitializeInterpreter(){
  BasicObjectPtr st = StackFrame::New(Binding::New());
  pimpl->current_frame = st;
  pimpl->root_frame = st;
  BasicObjectPtr bind = st->GetData<StackFrame>()->Binding();
  Library::BindPrimitiveObjects(bind);
}

void Interpreter::DigIntoNewFrame(const ptr_vector& expressions,
                                  const BasicObjectPtr& binding){
  BasicObjectPtr old_frame_object = pimpl->current_frame;

  BasicObjectPtr new_frame_object = StackFrame::New(binding);
  StackFrame* new_frame = new_frame_object->GetData<StackFrame>();
  LOG << "Step in: " << new_frame_object->Inspect();
  new_frame->SetUpperStack(old_frame_object);
  pimpl->current_frame = new_frame_object;
  
  new_frame->Setup(expressions);
}

DEFINE_SRU_PROC(ContinationInvoke){
  // Move into target stack position.
  *Interpreter::Instance()->CurrentStackFrame() = *proc->Get("CurrentStackFrame")->GetData<StackFrame>();
  if(args.size() > 0){
    LOG << "Contination invoked with: " << args[0]->Inspect();
    return args[0];
  }else{
    LOG << "Contination invoked";
    return Library::Instance()->Nil();
  }
}

BasicObjectPtr Interpreter::GetContinationToEscapeFromCurrentStack(){
  BasicObjectPtr cont = CREATE_SRU_PROC(ContinationInvoke);
  cont->Set("CurrentStackFrame", CurrentStackFrame()->GetUpperStack());
  return cont;
}

StackFrame* Interpreter::CurrentStackFrame(){
  return pimpl->current_frame->GetData<StackFrame>();
}

StackFrame* Interpreter::RootStackFrame(){
  return pimpl->root_frame->GetData<StackFrame>();
}

BasicObjectPtr Interpreter::Eval(BasicObjectPtr ast){
  ptr_vector asts;
  asts.push_back(ast);
  CurrentStackFrame()->Setup(asts);
  LOG << "setuped";
  while(! CurrentStackFrame()->EndOfTrees()){
    LOG << "Step start";
    assert(CurrentStackFrame()->EvalNode());
    LOG << "Step end";
  } 
  return CurrentStackFrame()->ReturnValue();
}

BasicObjectPtr Interpreter::Eval(const string& str){
  // Paser.parse("str")
  ptr_vector args;
  // Parser
  args.push_back(RefExpression::New(NULL, "sru_parser"));
  // "str"
  args.push_back(StringExpression::New(str));
  BasicObjectPtr call_parser = CallExpression::New(
      RefExpression::New(RefExpression::New(NULL,"sru_parser"), "parse"), args);

  BasicObjectPtr ast = Eval(call_parser);
  if(ast == Library::Instance()->Nil()) {
    // TODO: Check more detail..
    LOG_ERROR << "Parse error";
    return NULL;
  }
  LOG_ERROR << "Parse OK : " << ast->Inspect();

  BasicObjectPtr result = Eval(ast);
  return result;
}

