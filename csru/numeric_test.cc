// Programing Language SRU
// Copyright(C) 2005-2009 IKeJI
// 

#include "testing.h"
#include <cassert>

#include "numeric.h"
#include "library.h"
#include "testing_sru.h"
#include "string.h"
#include "constants.h"
#include "logging.h"
#include "testing_ast.h"

using namespace std;
using namespace sru;
using namespace sru_test;

TEST(SRUNumeric_InitializeTest){
  BasicObjectPtr obj = SRUNumeric::New(3);
  assert(obj->Get(sym::klass()) == Library::Instance()->Numeric());
  assert(SRUNumeric::GetValue(obj) == 3);
  
  obj = SRUNumeric::New(4);
  assert(obj->Get(sym::klass()) == Library::Instance()->Numeric());
  assert(SRUNumeric::GetValue(obj) == 4);
}

TEST(SRUNumeric_InspectTest){
  LOG_ERROR << SRUNumeric::New(123)->Inspect();
  assert(SRUNumeric::New(123)->Inspect() == "<Numeric(123) findSlot:<Proc({ -- Native Code -- }) ... >>");
  LOG_ERROR << SRUNumeric::New(0)->Inspect();
  assert(SRUNumeric::New(0)->Inspect() == "<Numeric(0) findSlot:<Proc({ -- Native Code -- }) ... >>");
  LOG_ERROR << SRUNumeric::New(-123)->Inspect();
  assert(SRUNumeric::New(-123)->Inspect() == "<Numeric(-123) findSlot:<Proc({ -- Native Code -- }) ... >>");
}

TEST(SRUNumeric_ParseTest){
  const BasicObjectPtr res = Call(Library::Instance()->Numeric(),sym::parse(),
                                  SRUString::New(symbol("3")));
  assert(res.get());
  assert(res->Get(sym::klass()) == Library::Instance()->Numeric());
  assert(SRUNumeric::GetValue(res) == 3);
}

void check_has(const symbol& sym){
  assert(Library::Instance()->Numeric()->HasSlot(sym::instanceMethods()));
  assert(Library::Instance()->Numeric()->Get(sym::instanceMethods())->HasSlot(sym));
}

TEST(SRUNumeric_EqualTest){
  check_has(sym::equal());
}

TEST(SRUNumeric_EqualTestTrue){
  // {a = 123; a.equal(a,123);}()
  BasicObjectPtr proc = C(P(
        L("a", C(R(R("Numeric"),"parse"), R("Numeric"), S("123"))),
        C(R(R("a"),"equal"),R("a"),C(R(R("Numeric"),"parse"), R("Numeric"), S("123")))
        ));
  LOG << InspectAST(proc);
  assert(InspectAST(proc) == "{"
      "(a = (Numeric).parse(Numeric, \"123\"));"
      "(a).equal(a, (Numeric).parse(Numeric, \"123\"));"
      "}()");
  BasicObjectPtr result = Interpreter::Instance()->Eval(proc);
  assert(result.get());
  assert(result != Library::Instance()->Nil());
  assert(result == Library::Instance()->True());
}

TEST(SRUNumeric_EqualTestFalse){
  // {a = 123; a.equal(a,456);}()
  BasicObjectPtr proc = C(P(
        L("a", C(R(R("Numeric"),"parse"), R("Numeric"), S("123"))),
        C(R(R("a"),"equal"),R("a"),C(R(R("Numeric"),"parse"), R("Numeric"), S("456")))
        ));
  LOG << InspectAST(proc);
  assert(InspectAST(proc) == "{"
      "(a = (Numeric).parse(Numeric, \"123\"));"
      "(a).equal(a, (Numeric).parse(Numeric, \"456\"));"
      "}()");
  BasicObjectPtr result = Interpreter::Instance()->Eval(proc);
  assert(result.get());
  assert(result != Library::Instance()->Nil());
  assert(result == Library::Instance()->False());
}

TEST(SRUNumeric_NotEqualTest){
  check_has(sym::notEqual());
}

TEST(SRUNumeric_NotEqualTestTrue){
  // {a = 123; a.notEqual(a,456);}()
  BasicObjectPtr proc = C(P(
        L("a", C(R(R("Numeric"),"parse"), R("Numeric"), S("123"))),
        C(R(R("a"),"notEqual"),R("a"),C(R(R("Numeric"),"parse"), R("Numeric"), S("456")))
        ));
  LOG << InspectAST(proc);
  assert(InspectAST(proc) == "{"
      "(a = (Numeric).parse(Numeric, \"123\"));"
      "(a).notEqual(a, (Numeric).parse(Numeric, \"456\"));"
      "}()");
  BasicObjectPtr result = Interpreter::Instance()->Eval(proc);
  assert(result.get());
  assert(result != Library::Instance()->Nil());
  assert(result == Library::Instance()->True());
}

TEST(SRUNumeric_NotEqualTestFalse){
  // {a = 123; a.notEqual(a,123);}()
  BasicObjectPtr proc = C(P(
        L("a", C(R(R("Numeric"),"parse"), R("Numeric"), S("123"))),
        C(R(R("a"),"notEqual"),R("a"),C(R(R("Numeric"),"parse"), R("Numeric"), S("123")))
        ));
  LOG << InspectAST(proc);
  assert(InspectAST(proc) == "{"
      "(a = (Numeric).parse(Numeric, \"123\"));"
      "(a).notEqual(a, (Numeric).parse(Numeric, \"123\"));"
      "}()");
  BasicObjectPtr result = Interpreter::Instance()->Eval(proc);
  assert(result.get());
  assert(result != Library::Instance()->Nil());
  assert(result == Library::Instance()->False());
}

TEST(SRUNumeric_GreaterThanTest){
  check_has(sym::greaterThan());
}

TEST(SRUNumeric_GreaterThanTestTrue){
  // {a = 456; a.greaterThan(a,123);}()
  BasicObjectPtr proc = C(P(
        L("a", C(R(R("Numeric"),"parse"), R("Numeric"), S("456"))),
        C(R(R("a"),"greaterThan"),R("a"),C(R(R("Numeric"),"parse"), R("Numeric"), S("123")))
        ));
  LOG << InspectAST(proc);
  assert(InspectAST(proc) == "{"
      "(a = (Numeric).parse(Numeric, \"456\"));"
      "(a).greaterThan(a, (Numeric).parse(Numeric, \"123\"));"
      "}()");
  BasicObjectPtr result = Interpreter::Instance()->Eval(proc);
  assert(result.get());
  assert(result != Library::Instance()->Nil());
  assert(result == Library::Instance()->True());
}

TEST(SRUNumeric_GreaterThanTestFalse){
  // {a = 123; a.greaterThan(a,456);}()
  BasicObjectPtr proc = C(P(
        L("a", C(R(R("Numeric"),"parse"), R("Numeric"), S("123"))),
        C(R(R("a"),"greaterThan"),R("a"),C(R(R("Numeric"),"parse"), R("Numeric"), S("456")))
        ));
  LOG << InspectAST(proc);
  assert(InspectAST(proc) == "{"
      "(a = (Numeric).parse(Numeric, \"123\"));"
      "(a).greaterThan(a, (Numeric).parse(Numeric, \"456\"));"
      "}()");
  BasicObjectPtr result = Interpreter::Instance()->Eval(proc);
  assert(result.get());
  assert(result != Library::Instance()->Nil());
  assert(result == Library::Instance()->False());
}

TEST(SRUNumeric_LessThanTest){
  check_has(sym::lessThan());
}

TEST(SRUNumeric_LessThanTestTrue){
  // {a = 123; a.lessThan(a,456);}()
  BasicObjectPtr proc = C(P(
        L("a", C(R(R("Numeric"),"parse"), R("Numeric"), S("123"))),
        C(R(R("a"),"lessThan"),R("a"),C(R(R("Numeric"),"parse"), R("Numeric"), S("456")))
        ));
  LOG << InspectAST(proc);
  assert(InspectAST(proc) == "{"
      "(a = (Numeric).parse(Numeric, \"123\"));"
      "(a).lessThan(a, (Numeric).parse(Numeric, \"456\"));"
      "}()");
  BasicObjectPtr result = Interpreter::Instance()->Eval(proc);
  assert(result.get());
  assert(result != Library::Instance()->Nil());
  assert(result == Library::Instance()->True());
}

TEST(SRUNumeric_LessThanTestFalse){
  // {a = 456; a.greaterThan(a,123);}()
  BasicObjectPtr proc = C(P(
        L("a", C(R(R("Numeric"),"parse"), R("Numeric"), S("456"))),
        C(R(R("a"),"lessThan"),R("a"),C(R(R("Numeric"),"parse"), R("Numeric"), S("123")))
        ));
  LOG << InspectAST(proc);
  assert(InspectAST(proc) == "{"
      "(a = (Numeric).parse(Numeric, \"456\"));"
      "(a).lessThan(a, (Numeric).parse(Numeric, \"123\"));"
      "}()");
  BasicObjectPtr result = Interpreter::Instance()->Eval(proc);
  assert(result.get());
  assert(result != Library::Instance()->Nil());
  assert(result == Library::Instance()->False());
}

TEST(SRUNumeric_PlusTest){
  check_has(sym::plus());
}

TEST(SRUNumeric_PlusTest1){
  // {a = 123; a.plus(a,456);}()
  BasicObjectPtr proc = C(P(
        L("a", C(R(R("Numeric"),"parse"), R("Numeric"), S("123"))),
        C(R(R("a"),"plus"),R("a"),C(R(R("Numeric"),"parse"), R("Numeric"), S("456")))
        ));
  LOG << InspectAST(proc);
  assert(InspectAST(proc) == "{"
      "(a = (Numeric).parse(Numeric, \"123\"));"
      "(a).plus(a, (Numeric).parse(Numeric, \"456\"));"
      "}()");
  BasicObjectPtr result = Interpreter::Instance()->Eval(proc);
  assert(result.get());
  assert(result != Library::Instance()->Nil());
  assert(SRUNumeric::GetValue(result) == 123+456);
}

TEST(SRUNumeric_MinousTest){
  check_has(sym::minus());
}

TEST(SRUNumeric_MinousTest1){
  // {a = 123; a.minus(a,456);}()
  BasicObjectPtr proc = C(P(
        L("a", C(R(R("Numeric"),"parse"), R("Numeric"), S("123"))),
        C(R(R("a"),"minus"),R("a"),C(R(R("Numeric"),"parse"), R("Numeric"), S("456")))
        ));
  LOG << InspectAST(proc);
  assert(InspectAST(proc) == "{"
      "(a = (Numeric).parse(Numeric, \"123\"));"
      "(a).minus(a, (Numeric).parse(Numeric, \"456\"));"
      "}()");
  BasicObjectPtr result = Interpreter::Instance()->Eval(proc);
  assert(result.get());
  assert(result != Library::Instance()->Nil());
  assert(SRUNumeric::GetValue(result) == 123-456);
}

TEST(SRUNumeric_AsteriskTest){
  check_has(sym::asterisk());
}

TEST(SRUNumeric_AsteriskTest1){
  // {a = 123; a.asterisk(a,456);}()
  BasicObjectPtr proc = C(P(
        L("a", C(R(R("Numeric"),"parse"), R("Numeric"), S("123"))),
        C(R(R("a"),"asterisk"),R("a"),C(R(R("Numeric"),"parse"), R("Numeric"), S("456")))
        ));
  LOG << InspectAST(proc);
  assert(InspectAST(proc) == "{"
      "(a = (Numeric).parse(Numeric, \"123\"));"
      "(a).asterisk(a, (Numeric).parse(Numeric, \"456\"));"
      "}()");
  BasicObjectPtr result = Interpreter::Instance()->Eval(proc);
  assert(result.get());
  assert(result != Library::Instance()->Nil());
  assert(SRUNumeric::GetValue(result) == 123*456);
}

TEST(SRUNumeric_SlashTest){
  check_has(sym::slash());
}

TEST(SRUNumeric_SlashTest1){
  // {a = 123; a.slash(a,456);}()
  BasicObjectPtr proc = C(P(
        L("a", C(R(R("Numeric"),"parse"), R("Numeric"), S("456"))),
        C(R(R("a"),"slash"),R("a"),C(R(R("Numeric"),"parse"), R("Numeric"), S("123")))
        ));
  LOG << InspectAST(proc);
  assert(InspectAST(proc) == "{"
      "(a = (Numeric).parse(Numeric, \"456\"));"
      "(a).slash(a, (Numeric).parse(Numeric, \"123\"));"
      "}()");
  BasicObjectPtr result = Interpreter::Instance()->Eval(proc);
  assert(result.get());
  assert(result != Library::Instance()->Nil());
  assert(SRUNumeric::GetValue(result) == 456/123);
}
