// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include "proc.h"

#include <cassert>
#include <vector>
#include "ast.h"
#include "interpreter.h"
#include "testing_ast.h"
#include "library.h"
#include "string.h"
#include "logging.h"
#include "numeric.h"

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
  LOG << InspectAST(call);
  assert(InspectAST(call) == "{Class;}()");
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
  LOG << InspectAST(call);
  assert(InspectAST(call) == "{|a|a;}(Class)");
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
  LOG << InspectAST(call);
  assert(InspectAST(call) == "{(a = Class);{a;};}()");
  BasicObjectPtr call2 = CallExpression::New(call,arg);
  LOG << InspectAST(call2);
  assert(InspectAST(call2) == "{(a = Class);{a;};}()()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(call2);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Proc_NewEvalTest){
  // " { Class }() "
  BasicObjectPtr call = C(P(R("Class")));
  LOG << InspectAST(call);
  assert(InspectAST(call) == "{Class;}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(call);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Proc_NewEvalTest2){
  // " {|a| a }(Class) "
  BasicObjectPtr call = C(P("a",R("a")),R("Class"));
  LOG << InspectAST(call);
  assert(InspectAST(call) == "{|a|a;}(Class)");
  BasicObjectPtr r = Interpreter::Instance()->Eval(call);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Proc_NewEvalTest3){
  // " { a = Class;{a} }()() "
  BasicObjectPtr call2 = C(C(P(L("a",R("Class")),P(R("a")))));
  LOG << InspectAST(call2);
  assert(InspectAST(call2) == "{(a = Class);{a;};}()()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(call2);
  assert(r.get());
  assert(r == Library::Instance()->Class());
}

TEST(Proc_InspectTest){
  // " { a = Class;{a} } "
  BasicObjectPtr m = P(L("a",R("Class")),P(R("a")));
  LOG << InspectAST(m);
  assert(InspectAST(m) == "{(a = Class);{a;};}");
  BasicObjectPtr r = Interpreter::Instance()->Eval(m);
  assert(r.get());
  LOG << r->Inspect();
  assert(r->Inspect() == "<Proc: {(a = Class);{a;};}>");
}

TEST(Proc_ContinationTest){
  // " {a = "1";{|:r| a="2"; r(); a="3"}(); a}() "
  BasicObjectPtr call2 = C(P(
      L("a", S("1")),     // a = "1"
      C(B("r",
          L("a", S("2")), // a = "2"
          C(R("r")),
          L("a", S("3"))  // a = "3"
        )),
      R("a")));
  LOG << InspectAST(call2);
  assert(InspectAST(call2) == 
      "{"
        "(a = \"1\");"
        "{|:r|"
          "(a = \"2\");"
          "r();"
          "(a = \"3\");"
        "}();"
        "a;"
      "}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(call2);
  assert(r.get());
  assert(SRUString::GetValue(r) == "2");
}

TEST(Proc_ContinationComplexAndJumpInTest){
  // {|:q|
  //   c = {|:r|
  //     {|:s|
  //       r(s);  ------+
  //     }();           |
  //          <-----+   |
  //     q("1"); ---|---|--+
  //   }();         |   |  |
  //        <-------|---/  |
  //   c();  -------/      |
  //   "2";                |
  // }();                  |
  //        <--------------/
  //
  BasicObjectPtr call2 = C(B("q",    // {|:q|
        L("c", C(B("r",              //   c = {|:r|
          C(B("s",                   //     {|:s|
            C(R("r"),R("s"))         //       r(s);
          )),                        //     }();
          C(R("q"),S("1"))           //     q("1");
          ))),                       //   }();
        C(R("c")),                   //   c();
        S("2")                       //   "2";
        ));                          // }();
  LOG << InspectAST(call2);
  assert(InspectAST(call2) == 
      "{|:q|"
        "(c = {|:r|"
          "{|:s|"
            "r(s);"
          "}();"
          "q(\"1\");"
        "}());"
        "c();"
        "\"2\";"
      "}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(call2);
  assert(r.get());
  assert(SRUString::GetValue(r) == "1");
}

// TODO: test instance methods "whileTrue".

TEST(Proc_LoopTest){
  // {a = 0; {|:break| c = { a = a + 1; r = a > 10; r.ifTrue({break()}); }; c.loop() }(); a;}()
  BasicObjectPtr loop_test = C(P(
        L("a", C(R(R("Numeric"),"parse"),R("Numeric"),S("0"))),
        C(B("break",
            L("c",P(
                L("a", C(R(R("a"),"plus"),R("a"),C(R(R("Numeric"),"parse"),R("Numeric"),S("1")))),
                L("r", C(R(R("a"),"greaterThan"),R("a"),C(R(R("Numeric"),"parse"),R("Numeric"),S("10")))),
                C(R(R("r"),"ifTrue"),R("r"),P(C(R("break"),R("a"))))
            )),
            C(R(R("c"),"loop"),R("c"))
        ))
  ));
  LOG << InspectAST(loop_test);
  assert(InspectAST(loop_test) == "{"
      "(a = (Numeric).parse(Numeric, \"0\"));"
      "{|:break|"
        "(c = {"
          "(a = (a).plus(a, (Numeric).parse(Numeric, \"1\")));"
          "(r = (a).greaterThan(a, (Numeric).parse(Numeric, \"10\")));"
          "(r).ifTrue(r, {break(a);});"
        "});"
        "(c).loop(c);"
      "}();"
    "}()");
  BasicObjectPtr r = Interpreter::Instance()->Eval(loop_test);
  assert(r.get());
  assert(SRUNumeric::GetValue(r) == 11);
}
