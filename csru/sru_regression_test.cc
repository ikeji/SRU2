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
      LOG_ERROR << "SetupSrc: " << before;
      BasicObjectPtr r = Interpreter::Instance()->Eval(before);
      assert(r.get());
      LOG_ERROR << "Setup: " << before << " #=> " << r->Inspect();
    }
    clock_t start = clock();
    LOG_ERROR << "Src: " << src;
    BasicObjectPtr r = Interpreter::Instance()->Eval(src);
    assert(r.get());
    LOG_ERROR << "ExpectResult:" << result;
    LOG_ERROR << "Result      :" << r->Inspect();
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
TEST_CODE(lambda,"{x}", "<Proc({x;})>");
TEST_CODE(lambda2,"{|a|x}", "<Proc({|a|x;})>");
TEST_CODE(lambda3,"{|a,b|x}", "<Proc({|a,b|x;})>");
TEST_CODE(lambda4,"{||x}", "<Proc({x;})>");
TEST_CODE(lambda5,"{a|x}", "<Proc({|a|x;})>");
TEST_CODE(lambda6,"{a,b|x}", "<Proc({|a,b|x;})>");
TEST_CODE(nil, "nil", "<nil>");
TEST_CODE(klass, "Class", "<Class class:<Class ... > findSlotMethod:<Proc({ -- Native Code -- }) ... >>");
TEST_CODE(call, "{3}()", "<Numeric(3)>");
TEST_CODE(call2, "{|x|x}(3)", "<Numeric(3)>");
TEST_CODE(nulllambda,"{}", "<Proc({})>");
TEST_CODE(nulllambdacall,"{}()", "<nil>");
TEST_CODE(inst_call, "3.plus(2)", "<Numeric(5)>");
TEST_CODE(inst_call_call, "3.invert().invert()", "<Numeric(3)>");
TEST_CODE(let,"x = 3", "<Numeric(3)>");
TEST_CODE2(let_result, "x = 3", "x", "<Numeric(3)>");
TEST_CODE2(native_field, "x = 3", "x.plus", "<Proc({ -- Native Code -- })>");
TEST_CODE(native_field2, "3.plus", "<Proc({ -- Native Code -- })>");
TEST_CODE(call3, "(3.plus)(3,3)", "<Numeric(6)>");
TEST_CODE2(call4, "x = 3", "(x.plus)(x,3)", "<Numeric(6)>");
TEST_CODE(let_func, "t={|x|(x.plus)(x,3)}", "<Proc({|x|(x).plus(x, (Numeric).parse(Numeric, \"3\"));})>");
TEST_CODE2(call_func, "t={|x|(x.plus)(x,3)}", "t(5)", "<Numeric(8)>");
TEST_CODE(let_func2, "plus={|x,y|x+y}", "<Proc({|x,y|{|$$|($$).plus($$, y);}(x);})>");
TEST_CODE2(use_func2, "plus={|x,y|x+y}", "plus(4,3)", "<Numeric(7)>");
TEST_CODE2(use_func3, "plus={|x,y|x+y}", "plus(plus(4,3),plus(1,2))", "<Numeric(10)>");
TEST_CODE2(use_func4, "plus={|x,y|x+y}", "plus(4,3).plus(plus(1,2))", "<Numeric(10)>");
TEST_CODE(true_false, "(true.ifTrueFalse)(true,{1},{2})", "<Numeric(1)>");
TEST_CODE(true_false2, "(false.ifTrueFalse)(true,{1},{2})", "<Numeric(2)>");
TEST_CODE(true_false3, "true.ifTrueFalse({1},{2})", "<Numeric(1)>");
TEST_CODE(true_false4, "false.ifTrueFalse({1},{2})", "<Numeric(2)>");
TEST_CODE(if_statement , "if ( 1 == 1 ) 3 end", "<Numeric(3)>");
TEST_CODE(if_statement2, "if ( 1 == 2 ) 3 end", "<nil>");
TEST_CODE(if_statement3, "if ( 1 == 1 ) 3 else 4 end", "<Numeric(3)>");
TEST_CODE(if_statement4, "if ( 1 == 2 ) 3 else 4 end", "<Numeric(4)>");
TEST_CODE(if_statement5, "if ( 1 == 2 ) 3 elsif ( 1 == 1 ) 4 end", "<Numeric(4)>");
TEST_CODE(if_statement6, "if ( 1 == 2 ) 3 elsif ( 1 == 1 ) 4 else 5 end", "<Numeric(4)>");
TEST_CODE(if_statement7, "if ( 1 == 2 ) 3 elsif ( 1 == 6 ) 4 else 5 end", "<Numeric(5)>");
TEST_CODE(if_statement8, "if ( 1 == 2 ) 3 elsif ( 1 == 6 ) 4 elsif ( 1 == 7 )  5 end", "<nil>");
TEST_CODE(if_statement9, "if ( 1 == 2 ) 3 elsif ( 1 == 6 ) 4 elsif ( 1 == 7 )  5 else 6 end", "<Numeric(6)>");
TEST_CODE(add, "1+4", "<Numeric(5)>");
TEST_CODE(sub, "1-4", "<Numeric(-3)>");
TEST_CODE(mul, "1*4", "<Numeric(4)>");
TEST_CODE(div, "5/2", "<Numeric(2)>");
// TODO: add test for || && == === !+ > < >= <= | & << >> ! ~
/*
TEST_CODE(let_proc, "s = {|x|x;}", "<Proc({|x|x;})>");
TEST_CODE2(let_proc_result, "s = {|x|x;}",
           "s",
           "<Proc({|x|x;})>");
TEST_CODE2(let_proc_result2, "s = {|x|x;}",
           "s(4)",
           "<Numeric(4)>");
TEST_CODE2(result_field, "s = {|x|x;}",
           "s(4).plus",
           "<Proc({ -- Native Code -- })>");
*/
