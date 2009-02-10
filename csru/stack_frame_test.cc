// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include "stack_frame.h"

#include <cassert>
#include <iostream>
#include "object_vector.h"
#include "ast.h"

using namespace sru;
using namespace std;

TEST(StackFrame_Backup){
  StackFrame st;
  StackFrame st2;
  st = st2;
}

TEST(StackFrame_EvalStep){
  StackFrame st;
  ptr_vector ast;
  ast.push_back(BasicObject::New(new StringExpression("Hello")).get());
  st.Setup(ast);
  int counter = 0;
  while(! st.EndOfTrees()){
    assert(st.EvalNode());
    counter++;
  }
  // Step1: Extract Tree
  // Step2: Eval StringExpression
  if(counter != 2) cout << "counter :" << counter << endl;
  assert(counter == 2);
}