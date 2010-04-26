// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#include <ctime>

#include "testing.h"
#include "logging.h"
#include "basic_object.h"
#include "interpreter.h"

using namespace sru;

class CodeTestCase : public sru_test::TestCase {
 public:
  CodeTestCase(const char* name,
               const char* src,
               const char* result,
               const char* before=NULL):
                 TestCase(name),
                 before(before),
                 src(src),
                 result(result){}
  virtual void operator()(){
    if(before != NULL){
      BasicObjectPtr r = Interpreter::Instance()->Eval(before);
      assert(r.get());
      LOG_ERROR << "Setup: " << before << " #=> " << r->Inspect();
    }
    clock_t start = clock();
    LOG_ERROR << src;
    BasicObjectPtr r = Interpreter::Instance()->Eval(src);
    assert(r.get());
    LOG_ERROR << r->Inspect();
    assert(r->Inspect() == result);
    LOG_ALWAYS << "time : " << ((static_cast<double>(clock() - start)) / CLOCKS_PER_SEC);
  }
  const char* before;
  const char* src;
  const char* result;
  CodeTestCase(const CodeTestCase& obj);
  CodeTestCase &operator=(const CodeTestCase&obj);

};


#define TEST_CODE(name, src, result) \
  CodeTestCase TEST_##name("Regression_" #name, src, result)
#define TEST_CODE2(name, before, src, result) \
  CodeTestCase TEST_##name("Regression_" #name, src, result, before)

TEST_CODE(number,"1", "<Numeric(1)>");
TEST_CODE(number2,"123", "<Numeric(123)>");
TEST_CODE(string,"\"a\"", "<String(\"a\")>");
TEST_CODE(string2,"\"abc\"", "<String(\"abc\")>");
/*
TEST_CODE(lambda,"{|x|x;}", "<Proc: {|x|x;}>");
TEST_CODE(let,"x = 3", "<Numeric: 1 >");
TEST_CODE2(let-result, "x = 3", "x", "<Numeric: 3 >");
TEST_CODE(let-proc, "s = {|x|x;}", "<Proc: {|x|x;}>");
TEST_CODE2(let-proc-result, "s = {|x|x;}",
           "s",
           "<Proc: {|x|x;}>");
TEST_CODE2(let-proc-result, "s = {|x|x;}",
           "s(4)",
           "<Numeric: 4 >");
TEST_CODE2(native-field, "x = 3", "x.plus", "<Proc: { -- Native Code -- }>");
TEST_CODE(native-field2, "3.plus", "<Proc: { -- Native Code -- }>");
TEST_CODE2(result-field, "s = {|x|x;}",
           "s(4).plus",
           "<Proc: { -- Native Code -- }>");
TEST_CODE(call, "(3.plus)(3,3)", "<Numeric: 6 >");
TEST_CODE2(call2, "x = 3", "(x.plus)(x,3)", "<Numeric: 6 >");
TEST_CODE(let-func, "t={|x|(x.plus)(x,3)}", "<Proc: {|x|(x).plus(x, (Numeric).parse(Numeric, \"3\"));}>");
TEST_CODE2(call-func, "t={|x|(x.plus)(x,3)}", "t(5)", "<Numeric: 8 >");
TEST_CODE(let-func2, "plus={|x,y|x+y}", "<Proc: {|x,y|(x).plus(x, y);}>");
TEST_CODE2(use-func2, "plus={|x,y|x+y}", "plus(4,3)", "<Numeric: 7 >");
TEST_CODE2(use-func3, "plus={|x,y|x+y}", "plus(plus(4,3),plus(1,2))", "<Numeric: 10 >");
TEST_CODE2(use-func4, "plus={|x,y|x+y}", "plus(4,3).plus(plus(4,3),plus(1,2))", "<Numeric: 10 >");
TEST_CODE(true-false, "(true.ifTrueFalse)(true,{1},{2})", "<Numeric: 1 >");
TEST_CODE(true-false2, "(false.ifTrueFalse)(true,{1},{2})", "<Numeric: 2 >");
TEST_CODE(true-false3, "true.ifTrueFalse({1},{2})", "<Numeric: 1 >");
TEST_CODE(true-false4, "false.ifTrueFalse({1},{2})", "<Numeric: 2 >");
*/
