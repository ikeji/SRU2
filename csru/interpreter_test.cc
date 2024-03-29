// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "interpreter.h"

#include "testing.h"
#include "testing_ast.h"
#include <cassert>

#include <string>
#include "basic_object.h"
#include "ast.h"
#include "sru_string.h"
#include "library.h"
#include "constants.h"
#include "logging.h"
#include "symbol.h"

using namespace sru;
using namespace sru_test;
using namespace std;

TEST(Interpreter_StringExpressionTest){
  string str = "Hello";
  StringExpression* se = new StringExpression(symbol(str));
  BasicObjectPtr p = BasicObject::New(se);
  LOG << se->InspectAST();
  assert(se->InspectAST() == "\"Hello\"");
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  string rstr = SRUString::GetValue(r.get()).to_str();
  if(!( str == rstr) ){
    LOG << "str:"  << str;
    LOG << "rstr:" << rstr;
  }
  assert(str == rstr);
}

TEST(Interpreter_ComplexExpressionTest){
  string str = "Hello";
  // { "hello" }()
  ptr_vector  args;
  vector<symbol> vargs;
  ptr_vector expressions;
  expressions.push_back(StringExpression::New(symbol(str)));
  BasicObjectPtr p = 
  CallExpression::New(
    ProcExpression::New(
      vargs,
      symbol(""),
      expressions
    ),
    args
  );
  LOG << InspectAST(p);
  assert(InspectAST(p) == "{\"Hello\";}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  string rstr = SRUString::GetValue(r.get()).to_str();
  if(!( str == rstr) ){
    LOG << "str:"  << str;
    LOG << "rstr:" << rstr;
  }
  assert(str == rstr);
}

TEST(Interpreter_RefExpressionTest){
  // " Class "
  BasicObjectPtr p = RefExpression::New(NULL,SRUString::New(sym::Class()));
  LOG << InspectAST(p);
  assert(InspectAST(p) == "Class");
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Interpreter_LetExpressionTest){
  // " hoge = Class "
  BasicObjectPtr ref = RefExpression::New(NULL,SRUString::New(sym::Class()));
  BasicObjectPtr p = LetExpression::New(NULL,SRUString::New(symbol("hoge")),ref);
  LOG << InspectAST(p);
  assert(InspectAST(p) == "(hoge = Class)");
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  assert(r == Library::Instance()->Class());
  // " hoge "
  p = RefExpression::New(NULL,SRUString::New(symbol("hoge")));
  LOG << InspectAST(p);
  assert(InspectAST(p) == "hoge");
  r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Interpreter_StringExpressionRegTest){
  string str = "Hello";
  string code = "\"Hello\"";
  BasicObjectPtr r = Interpreter::Instance()->Eval(code);
  assert(r.get());
  string rstr = SRUString::GetValue(r.get()).to_str();
  if(!( str == rstr) ){
    LOG << "str:"  << str;
    LOG << "rstr:" << rstr;
  }
  assert(str == rstr);
}

TEST(Interpreter_ComplexExpressionRegTest){
  LOG << "TODO: enable this test after imprement Parser object";
  return;
  string str = "Hello";
  string code = "{ \"Hello\" }()";
  BasicObjectPtr r = Interpreter::Instance()->Eval(code);
  assert(r.get());
  string rstr = SRUString::GetValue(r.get()).to_str();
  if(!( str == rstr) ){
    LOG << "str:"  << str;
    LOG << "rstr:" << rstr;
  }
  assert(str == rstr);
}

TEST(Interpreter_RefExpressionRegTest){
  // " Class "
  string code = " Class ";
  BasicObjectPtr r = Interpreter::Instance()->Eval(code);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Interpreter_LetExpressionRegTest){
  LOG << "TODO: enable this test after imprement Parser object";
  return;
  // " hoge = Class "
  string code = " hoge = Class ";
  BasicObjectPtr r = Interpreter::Instance()->Eval(code);
  assert(r.get());
  assert(r == Library::Instance()->Class());
  // " hoge "
  code = " hoge ";
  r = Interpreter::Instance()->Eval(code);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Interpreter_EvalCallExpressionRegTest){
  // NOTE: I found bug in stack_frame.cc .
  // (Numeric).parse((Numeric), "10")
  BasicObjectPtr p = C(R(R(sym::Numeric()),sym::parse()),
      R(sym::Numeric()), S("10"));
  LOG << InspectAST(p);
  assert(InspectAST(p) == "(Numeric).parse(Numeric, \"10\")");
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  assert(r->Get(sym::klass()) == Library::Instance()->Numeric());
}

