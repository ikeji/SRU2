// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include <cassert>
#include "ast.h"

#include <string>
#include <iostream>

using namespace sru;
using namespace std;

TEST(AST_StringExpressionTest){
  string test = "hogehoge";
  StringExpression* obj = new StringExpression(test);
  test = "foobar";
  cout << obj->InspectAST() << endl;
  assert(obj->InspectAST() == "\"hogehoge\"");
  assert(obj->String() != test);
  assert(obj->String() == "hogehoge");
}

