// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
//#include "binding.h"

#include <iostream>
#include <cassert>
#include "testing_ast.h"
#include "interpreter.h"
#include "library.h"

using namespace std;
using namespace sru_test;
using namespace sru;

TEST(Binding_ScopeTest){
  // { a = Class; {a}();}() => Class
  BasicObjectPtr p = C(P(L("a",R("Class")),C(P(R("a")))));
  cout << InspectAST(p) << endl;
  assert(InspectAST(p) == "{(a = Class);{a;}();}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Binding_ScopeTest2){
  // { a = Class; {|a|a}(String);}() => String 
  BasicObjectPtr p = C(P(L("a",R("Class")),C(P("a",R("a")),R("String"))));
  cout << InspectAST(p) << endl;
  assert(InspectAST(p) == "{(a = Class);{|a|a;}(String);}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  cout << "Result: " << r.get() << endl;
  assert(r.get());
  assert(r == Library::Instance()->String());
}

TEST(Binding_ScopeTest3){
  // { a = Class; {(a=true)}();a}() => true
  BasicObjectPtr p = C(P(L("a",R("Class")),C(P(L("a",R("true")))),R("a")));
  cout << InspectAST(p) << endl;
  assert(InspectAST(p) == "{(a = Class);{(a = true);}();a;}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  assert(r == Library::Instance()->True());
}

TEST(Binding_ScopeTest4){
  cout << "TODO: enable this test after impliment binding chain" << endl;
  return;
  // { a = Class; {|a|(a=true)}(String);a}() => Class
  BasicObjectPtr p = C(P(L("a",R("Class")),C(P("a",L("a",R("true"))),R("String")),R("a")));
  cout << InspectAST(p) << endl;
  assert(InspectAST(p) ==
         "{(a = Class);{|a|(a = true);}(String);a;}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  cout << "Result: " << r->Inspect() << endl;
  assert(r == Library::Instance()->Class());
}
