// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "interpreter.h"

#include <iostream>
#include <vector>
#include "ast.h"
#include "basic_object.h"
#include "object_vector.h"
#include "stack_frame.h"
#include "library.h"

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
  BasicObjectPtr st = StackFrame::New();
  pimpl->current_frame = st;
  pimpl->root_frame = st;
  BasicObjectPtr bind = st->GetData<StackFrame>()->Binding();
  Library::BindPrimitiveObjects(bind);
}

void Interpreter::DigIntoNewFrame(const ptr_vector& expressions){
  BasicObjectPtr old_frame_object = pimpl->current_frame;

  BasicObjectPtr new_frame_object = StackFrame::New();
  StackFrame* new_frame = new_frame_object->GetData<StackFrame>();
#ifdef DEBUG
  cout << "Step in: " << new_frame_object->Inspect() << endl;
#endif
  new_frame->SetUpperStack(old_frame_object);
  pimpl->current_frame = new_frame_object;
  
  new_frame->Setup(expressions);
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
#ifdef DEBUG
  cout << "setuped" << endl;
#endif
  while(! CurrentStackFrame()->EndOfTrees()){
#ifdef DEBUG
    cout << "Step start" << endl;
#endif
    assert(CurrentStackFrame()->EvalNode());
#ifdef DEBUG
    cout << "Step end" << endl;
#endif
  } 
  return CurrentStackFrame()->ReturnValue();
}

BasicObjectPtr Interpreter::Eval(const string& str){
  // Paser.parse("str")
  ptr_vector arg;
  // Parser
  arg.push_back(RefExpression::New(NULL, "sru_parser"));
  // "str"
  arg.push_back(StringExpression::New(str));
  BasicObjectPtr call_parser = CallExpression::New(
      RefExpression::New(RefExpression::New(NULL,"sru_parser"), "parse"), arg);

  BasicObjectPtr ast = Eval(call_parser);
  if(ast == Library::Instance()->Nil()) {
    // TODO: Check more detail..
    cout << "Parse error" << endl;
    return NULL;
  }

  BasicObjectPtr result = Eval(ast);
  return result;
}

