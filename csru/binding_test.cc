// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
//#include "binding.h"

#include <cassert>
#include "testing_ast.h"
#include "interpreter.h"
#include "library.h"
#include "logging.h"
#include "constants.h"

using namespace std;
using namespace sru_test;
using namespace sru;

TEST(Binding_ScopeTest){
  // { a = Class; {a}();}() => Class
  BasicObjectPtr p = C(P(L("a",R(sym::Class())),C(P(R("a")))));
  LOG << InspectAST(p);
  assert(InspectAST(p) == "{(a = Class);{a;}();}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  LOG << r->Inspect();
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Binding_ScopeTest2){
  // { a = Class; {|a|a}(String);}() => String 
  BasicObjectPtr p = C(P(
      L("a",R(sym::Class())),
      C(P("a",R("a")),
      R(sym::String()))));
  LOG << InspectAST(p);
  assert(InspectAST(p) == "{(a = Class);{|a|a;}(String);}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  LOG << "Result: " << r.get();
  assert(r.get());
  assert(r == Library::Instance()->String());
}

TEST(Binding_ScopeTest3){
  // { a = Class; {(a=true)}();a}() => true
  BasicObjectPtr p = C(P(
      L("a",R(sym::Class())),
      C(P(L("a",R(sym::tlue())))),
      R("a")));
  LOG << InspectAST(p);
  assert(InspectAST(p) == "{(a = Class);{(a = true);}();a;}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  assert(r == Library::Instance()->True());
}

TEST(Binding_ScopeTest4){
  // { a = Class; {|a|(a=true)}(String);a}() => Class
  BasicObjectPtr p = C(P(
      L("a",R(sym::Class())),
      C(P("a",L("a",R(sym::tlue()))),
      R(sym::String())),R("a")));
  LOG << InspectAST(p);
  assert(InspectAST(p) ==
         "{(a = Class);{|a|(a = true);}(String);a;}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(p);
  assert(r.get());
  LOG << "Result: " << r->Inspect();
  assert(r == Library::Instance()->Class());
}
