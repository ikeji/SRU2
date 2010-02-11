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
  assert(obj->Get(fCLASS) == Library::Instance()->Numeric());
  assert(SRUNumeric::GetValue(obj) == 3);
  
  obj = SRUNumeric::New(4);
  assert(obj->Get(fCLASS) == Library::Instance()->Numeric());
  assert(SRUNumeric::GetValue(obj) == 4);
}

TEST(SRUNumeric_InspectTest){
  LOG << SRUNumeric::New(123)->Inspect();
  assert(SRUNumeric::New(123)->Inspect() == "<Numeric: 123 >");
  assert(SRUNumeric::New(0)->Inspect() == "<Numeric: 0 >");
  assert(SRUNumeric::New(-123)->Inspect() == "<Numeric: -123 >");
}

TEST(SRUNumeric_ParseTest){
  const BasicObjectPtr res = Call(Library::Instance()->Numeric(),"parse",
                                  SRUString::New("3"));
  assert(res.get());
  assert(res->Get(fCLASS) == Library::Instance()->Numeric());
  assert(SRUNumeric::GetValue(res) == 3);
}

TEST(SRUNumeric_EqualTest){
  assert(Library::Instance()->Numeric()->HasSlot(fINSTANCE_METHODS));
  assert(Library::Instance()->Numeric()->Get(fINSTANCE_METHODS)->HasSlot("equal"));
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
  assert(Library::Instance()->Numeric()->HasSlot(fINSTANCE_METHODS));
  assert(Library::Instance()->Numeric()->Get(fINSTANCE_METHODS)->HasSlot("notEqual"));
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
  assert(Library::Instance()->Numeric()->HasSlot(fINSTANCE_METHODS));
  assert(Library::Instance()->Numeric()->Get(fINSTANCE_METHODS)->HasSlot("greaterThan"));
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
  assert(Library::Instance()->Numeric()->HasSlot(fINSTANCE_METHODS));
  assert(Library::Instance()->Numeric()->Get(fINSTANCE_METHODS)->HasSlot("lessThan"));
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
  assert(Library::Instance()->Numeric()->HasSlot(fINSTANCE_METHODS));
  assert(Library::Instance()->Numeric()->Get(fINSTANCE_METHODS)->HasSlot("plus"));
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
  assert(Library::Instance()->Numeric()->HasSlot(fINSTANCE_METHODS));
  assert(Library::Instance()->Numeric()->Get(fINSTANCE_METHODS)->HasSlot("minous"));
}

TEST(SRUNumeric_MinousTest1){
  // {a = 123; a.minous(a,456);}()
  BasicObjectPtr proc = C(P(
        L("a", C(R(R("Numeric"),"parse"), R("Numeric"), S("123"))),
        C(R(R("a"),"minous"),R("a"),C(R(R("Numeric"),"parse"), R("Numeric"), S("456")))
        ));
  LOG << InspectAST(proc);
  assert(InspectAST(proc) == "{"
      "(a = (Numeric).parse(Numeric, \"123\"));"
      "(a).minous(a, (Numeric).parse(Numeric, \"456\"));"
      "}()");
  BasicObjectPtr result = Interpreter::Instance()->Eval(proc);
  assert(result.get());
  assert(result != Library::Instance()->Nil());
  assert(SRUNumeric::GetValue(result) == 123-456);
}

TEST(SRUNumeric_AsteriskTest){
  assert(Library::Instance()->Numeric()->HasSlot(fINSTANCE_METHODS));
  assert(Library::Instance()->Numeric()->Get(fINSTANCE_METHODS)->HasSlot("asterisk"));
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
  assert(Library::Instance()->Numeric()->HasSlot(fINSTANCE_METHODS));
  assert(Library::Instance()->Numeric()->Get(fINSTANCE_METHODS)->HasSlot("slash"));
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
