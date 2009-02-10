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

using namespace sru;
using namespace std;

struct Interpreter::Impl {
  Impl():current_frame(BasicObject::New(new StackFrame())){
  }
  BasicObjectPtr current_frame;
};

Interpreter::Interpreter():pimpl(new Impl()){
}

Interpreter::~Interpreter(){
  delete pimpl;
}

BasicObjectPtr Interpreter::CurrentStackFrame(){
  return pimpl->current_frame;
}

BasicObjectPtr Interpreter::Eval(BasicObjectPtr ast){
  ptr_vector asts;
  asts.push_back(ast);
  StackFrame* st = dynamic_cast<StackFrame*>(pimpl->current_frame->Data());
  st->Setup(asts);
#ifdef DEBUG
  cout << "setuped" << endl;
#endif
  while(! st->EndOfTrees()){
#ifdef DEBUG
    cout << "exec" << endl;
#endif
    assert(st->EvalNode());
  } 
  return st->ReturnValue();
}

