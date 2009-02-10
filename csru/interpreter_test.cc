// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "interpreter.h"

#include "testing.h"
#include <cassert>

#include <string>
#include <iostream>
#include "basic_object.h"
#include "ast.h"
#include "string.h"
#include "library.h"

using namespace sru;
using namespace std;

TEST(Interpreter_StringExpressionTest){
  string str = "Hello";
  BasicObjectPtr p = BasicObject::New(new StringExpression(str));
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  string rstr = SRUString::GetValue(r.get());
  if(!( str == rstr) ){
    cout << "str:"  << str  << endl;
    cout << "rstr:" << rstr << endl;
  }
  assert(str == rstr);
}

TEST(Interpreter_ComplexExpressionTest){
  string str = "Hello";
  // { "hello" }()
  ptr_vector  args;
  vector<string> vargs;
  ptr_vector expressions;
  expressions.push_back(StringExpression::New(str));
  BasicObjectPtr p = 
  CallExpression::New(
    ProcExpression::New(
      vargs,
      string(""),
      expressions
    ),
    args
  );
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  string rstr = SRUString::GetValue(r.get());
  if(!( str == rstr) ){
    cout << "str:"  << str  << endl;
    cout << "rstr:" << rstr << endl;
  }
  assert(str == rstr);
}

TEST(Interpreter_RefExpressionTest){
  // " Class "
  BasicObjectPtr p = RefExpression::New(NULL,"Class");
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Interpreter_LetExpressionTest){
  // " hoge = Class "
  BasicObjectPtr ref = RefExpression::New(NULL,"Class");
  BasicObjectPtr p = LetExpression::New(NULL,"hoge",ref);
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  assert(r == Library::Instance()->Class());
  // " hoge "
  p = RefExpression::New(NULL,"hoge");
  r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}
