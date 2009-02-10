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
#include "testing_ast.h"
#include "library.h"

using namespace std;
using namespace sru;
using namespace sru_test;

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
  cout << dynamic_cast<Expression*>(call->Data())->Inspect() << endl;
  assert(dynamic_cast<Expression*>(call->Data())->Inspect() == 
         "{Class;}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(call);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Proc_EvalTest2){
  // " {|a| a }(Class) "
  BasicObjectPtr ref = RefExpression::New(NULL,"a");
  vector<string> varg;
  varg.push_back("a");
  string retval;
  ptr_vector expressions;
  expressions.push_back(ref);
  BasicObjectPtr proc = ProcExpression::New(varg,retval,expressions);
  BasicObjectPtr ref2 = RefExpression::New(NULL,"Class");
  ptr_vector arg;
  arg.push_back(ref2);
  BasicObjectPtr call = CallExpression::New(proc,arg);
  cout << dynamic_cast<Expression*>(call->Data())->Inspect() << endl;
  assert(dynamic_cast<Expression*>(call->Data())->Inspect() == 
         "{|a|a;}(Class)");
  BasicObjectPtr r = Interpreter::Instance()->Eval(call);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Proc_EvalTest3){
  // " { a = Class;{a} }()() "
  // let <= a = Class
  BasicObjectPtr ref = RefExpression::New(NULL,"Class");
  BasicObjectPtr let = LetExpression::New(NULL,"a",ref);
  // proc <= {a}
  BasicObjectPtr ref2 = RefExpression::New(NULL,"a");
  vector<string> varg;
  string retval;
  ptr_vector expressions;
  expressions.push_back(ref2);
  BasicObjectPtr proc = ProcExpression::New(varg,retval,expressions);
  // proc2 <= { 'let';'proc' }
  vector<string> varg2;
  string retval2;
  ptr_vector expressions2;
  expressions2.push_back(let);
  expressions2.push_back(proc);
  BasicObjectPtr proc2 = ProcExpression::New(varg2,retval2,expressions2);
  
  ptr_vector arg;
  BasicObjectPtr call = CallExpression::New(proc2,arg);
  BasicObjectPtr call2 = CallExpression::New(call,arg);
  cout << dynamic_cast<Expression*>(call2->Data())->Inspect() << endl;
  assert(dynamic_cast<Expression*>(call2->Data())->Inspect() == 
         "{(a = Class);{a;};}()()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(call2);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Proc_NewEvalTest){
  // " { Class }() "
  BasicObjectPtr call = C(P(R("Class")));
  cout << dynamic_cast<Expression*>(call->Data())->Inspect() << endl;
  assert(dynamic_cast<Expression*>(call->Data())->Inspect() == 
         "{Class;}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(call);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Proc_NewEvalTest2){
  // " {|a| a }(Class) "
  BasicObjectPtr call = C(P("a",R("a")),R("Class"));
  cout << dynamic_cast<Expression*>(call->Data())->Inspect() << endl;
  assert(dynamic_cast<Expression*>(call->Data())->Inspect() == 
         "{|a|a;}(Class)");
  BasicObjectPtr r = Interpreter::Instance()->Eval(call);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Proc_NewEvalTest3){
  // " { a = Class;{a} }()() "
  BasicObjectPtr call2 = C(C(P(L("a",R("Class")),P(R("a")))));
  cout << dynamic_cast<Expression*>(call2->Data())->Inspect() << endl;
  assert(dynamic_cast<Expression*>(call2->Data())->Inspect() == 
         "{(a = Class);{a;};}()()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(call2);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}
