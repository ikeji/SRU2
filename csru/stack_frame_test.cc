// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include "stack_frame.h"

#include <cassert>
#include "object_container.h"
#include "ast.h"
#include "binding.h"
#include "logging.h"

using namespace sru;
using namespace std;

TEST(StackFrame_BackupTest){
  StackFrame st(Binding::New());
  StackFrame st2(Binding::New());
  st = st2;
}

TEST(StackFrame_EvalStepTest){
  BasicObjectPtr s = StackFrame::New(Binding::New());
  StackFrame* st = s->GetData<StackFrame>();
  ptr_vector ast;
  ast.push_back(BasicObject::New(new StringExpression("Hello")));
  st->Setup(ast);
  int counter = 0;
  while(! st->EndOfTrees()){
    assert(st->EvalNode());
    counter++;
  }
  // Step1: Extract Tree
  // Step2: Eval StringExpression
  LOG << "counter :" << counter;
  assert(counter == 2);
}
