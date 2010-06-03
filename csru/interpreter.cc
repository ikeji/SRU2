// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "interpreter.h"

#include <vector>
#include "ast.h"
#include "basic_object.h"
#include "object_container.h"
#include "stack_frame.h"
#include "library.h"
#include "binding.h"
#include "native_proc.h"
#include "logging.h"
#include "constants.h"
#include "numeric.h"
#include "string.h"
#include "utils.h"

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
  StackFrame* stackframe = st->GetData<StackFrame>();
  assert(stackframe);
  BasicObjectPtr bind = stackframe->Binding();
  Library::BindPrimitiveObjects(bind);
}

void Interpreter::DigIntoNewFrame(const ptr_vector& expressions,
                                  const BasicObjectPtr& binding){
  BasicObjectPtr old_frame_object = pimpl->current_frame;

  BasicObjectPtr new_frame_object = StackFrame::New(binding);
  StackFrame* new_frame = new_frame_object->GetData<StackFrame>();
  assert(new_frame);
  LOG << "Step in: " << new_frame_object->Inspect();
  new_frame->SetUpperStack(old_frame_object);
  pimpl->current_frame = new_frame_object;
  
  new_frame->Setup(expressions);
}

DEFINE_SRU_PROC(ContinationInvoke){
  // Move into target stack position.
  StackFrame* cur_frame = proc->Get(sym::CurrentStackFrame())->GetData<StackFrame>();
  assert(cur_frame);
  *Interpreter::Instance()->CurrentStackFrame() = *cur_frame;
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
  cont->Set(sym::CurrentStackFrame(), CurrentStackFrame()->GetUpperStack());
  return cont;
}

StackFrame* Interpreter::CurrentStackFrame(){
  StackFrame* s = pimpl->current_frame->GetData<StackFrame>();
  assert(s);
  return s;
}

StackFrame* Interpreter::RootStackFrame(){
  StackFrame* s = pimpl->root_frame->GetData<StackFrame>();
  assert(s);
  return s;
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
  BasicObjectPtr result;
  string src = str;
  while(true) {
    LOG << "Start parse: " << src;
    // Paser.parse("str")
    ptr_vector args;
    // Parser
    args.push_back(RefExpression::New(NULL, SRUString::New(sym::sru_parser())));
    // "str"
    args.push_back(StringExpression::New(symbol(src.c_str())));
    BasicObjectPtr call_parser = CallExpression::New(
        RefExpression::New(
          RefExpression::New(NULL,SRUString::New(sym::sru_parser())),
          SRUString::New(sym::parse())), args);
  
    BasicObjectPtr obj = Eval(call_parser);
    if(!obj->HasSlot(sym::ast()) ||
       IsNil(obj->Get(sym::ast()))){
      if(obj->HasSlot(sym::pos())){
        PrintErrorPosition(src, SRUNumeric::GetValue(obj->Get(sym::pos())));
      }
      // TODO: Check more detail..
      LOG_ERROR << "Parse error: " <<
        SRUString::GetValue(
          obj->Get(sym::error())
        ).to_str();
      return NULL;
    }
    BasicObjectPtr ast = obj->Get(sym::ast());
    LOG_ERROR << "Parse OK : " << ast->Inspect();
  
    result = Eval(ast);

    size_t pos = SRUNumeric::GetValue(obj->Get(sym::pos()));
    if(pos == src.size()) break;
    src = src.substr(pos);
  }
  return result;
}

