// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include "proc.h"

#include <iostream>
#include <cassert>
#include <vector>
#include "ast.h"
#include "interpreter.h"
#include "library.h"

using namespace std;
using namespace sru;

TEST(Proc_NewTest){
  vector<string> varg;
  string retval;
  ptr_vector expressions;
  BasicObjectPtr binding = BasicObject::New();
  assert(Proc::New(varg,retval,expressions,binding).get());
}

TEST(Proc_EvalTest){
  // " { Class }() "
  BasicObjectPtr ref = RefExpression::New(NULL,"Class");
  vector<string> varg;
  string retval;
  ptr_vector expressions;
  expressions.push_back(ref);
  BasicObjectPtr proc = ProcExpression::New(varg,retval,expressions);
  ptr_vector arg;
  BasicObjectPtr call = CallExpression::New(proc,arg);
  BasicObjectPtr r = Interpreter::Instance()->Eval(call);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

