// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include "parser.h"

#include <cassert>
#include <string>
#include <cstdlib>
#include "testing.h"
#include "interpreter.h"
#include "basic_object.h"

#include <ctime>

using namespace std;
using namespace sru;
using namespace sru_test;

void TestCode(const string& code, const string& result){
  LOG_ERROR << code;
  BasicObjectPtr r = Interpreter::Instance()->Eval(code);
  assert(r.get());
  LOG_ERROR << r->Inspect();
  assert(r->Inspect() == result);
}

TEST(Scheme_Test1){
  clock_t start = clock();
  TestCode("1", "<Numeric: 1 >");
  TestCode("123", "<Numeric: 123 >");
  TestCode("\"a\"", "<\"a\">");
  TestCode("\"abc\"", "<\"abc\">");
  TestCode("(lambda (x) x)", "<Proc: {|x|x;}>");
  TestCode("(define x 3)", "<Numeric: 3 >");
  TestCode("x", "<Numeric: 3 >");
  TestCode("(define s (lambda (x) x))", "<Proc: {|x|x;}>");
  TestCode("s", "<Proc: {|x|x;}>");
  TestCode("(s 4)", "<Numeric: 4 >");
  TestCode("x.plus", "<Proc: { -- Native Code -- }>");
  TestCode("3.plus", "<Proc: { -- Native Code -- }>");
  TestCode("(s 4).plus", "<Proc: { -- Native Code -- }>");
  TestCode("(x.plus x 3)", "<Numeric: 6 >");
  TestCode("(define t (lambda (x) (x.plus x 3)))", "<Proc: {|x|(x).plus(x, (Numeric).parse(Numeric, \"3\"));}>");
  TestCode("(t 5)", "<Numeric: 8 >");
  TestCode("(define plus (lambda (x,y) (x.plus x y)))", "<Proc: {|x,y|(x).plus(x, y);}>");
  TestCode("(plus 4 3)", "<Numeric: 7 >");
  TestCode("(plus (plus 4 3) (plus 1 2))", "<Numeric: 10 >");
  TestCode("((plus 4 3).plus (plus 4 3) (plus 1 2))", "<Numeric: 10 >");
  TestCode("(true.ifTrueFalse true (lambda (x) 1) (lambda (x) 2))", "<Numeric: 1 >");
  TestCode("(false.ifTrueFalse false (lambda (x) 1) (lambda (x) 2))", "<Numeric: 2 >");
  start = clock();
  TestCode("(define fib (lambda (x) "
             "((x.lessThan x 2).ifTrueFalse "
               "(x.lessThan x 2)"
               "(lambda (n) 1) "
               "(lambda (n) "
                 "(1.plus (fib (x.minous x 1)) (fib (x.minous x 2))) "
               ") "
             ") "
           ") )", 
           "<Proc: {|x|"
             "((x).lessThan(x, (Numeric).parse(Numeric, \"2\"))).ifTrueFalse("
               "(x).lessThan(x, (Numeric).parse(Numeric, \"2\")), "
               "{|n|(Numeric).parse(Numeric, \"1\");}, "
               "{|n|"
                 "((Numeric).parse(Numeric, \"1\")).plus("
                   "fib((x).minous(x, (Numeric).parse(Numeric, \"1\"))), "
                   "fib((x).minous(x, (Numeric).parse(Numeric, \"2\")))"
                 ");"
               "}"
             ");"
           "}>");
  LOG_ERROR << "time : " << ((static_cast<double>(clock() - start)) / CLOCKS_PER_SEC);
  TestCode("(fib 0)", "<Numeric: 1 >");
  TestCode("(fib 1)", "<Numeric: 1 >");
  TestCode("(fib 2)", "<Numeric: 2 >");
  TestCode("(fib 3)", "<Numeric: 3 >");
  TestCode("(fib 4)", "<Numeric: 5 >");
  TestCode("(fib 5)", "<Numeric: 8 >");
  start = clock();
  TestCode("(fib 10)", "<Numeric: 89 >");
  LOG_ERROR << "time : " << ((static_cast<double>(clock() - start)) / CLOCKS_PER_SEC);
  start = clock();
  TestCode("(fib 20)", "<Numeric: 10946 >");
  LOG_ERROR << "time : " << ((static_cast<double>(clock() - start)) / CLOCKS_PER_SEC);
  LOG_ERROR << "TODO: enable tak function.";
  return;
  start = clock();
  TestCode("(define tak (lambda (x,y,z) "
             "((x.lessThan x (y.plus y 1)).ifTrueFalse "
               "Nil "
               "(lambda (n) y) "
               "(lambda (n) "
                 "(tak "
                   "(tak (x.minous x 1) y z) "
                   "(tak (y.minous y 1) z x) "
                   "(tak (z.minous z 1) x y) "
                 ") "
               ") "
             ")"
           "))",
           "<Proc: {|x,y,z|"
             "((x).lessThan(x, (y).plus(y, (Numeric).parse(Numeric, \"1\")))).ifTrueFalse("
               "Nil, "
               "{|n|y;}, "
               "{|n|"
                 "tak("
                   "tak((x).minous(x, (Numeric).parse(Numeric, \"1\")), y, z), "
                   "tak((y).minous(y, (Numeric).parse(Numeric, \"1\")), z, x), "
                   "tak((z).minous(z, (Numeric).parse(Numeric, \"1\")), x, y));"
               "});"
           "}>");
  LOG_ERROR << "time : " << ((static_cast<double>(clock() - start)) / CLOCKS_PER_SEC);
}
