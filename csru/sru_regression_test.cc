// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#include <ctime>
#include <string>

#include "testing.h"
#include "logging.h"
#include "basic_object.h"
#include "interpreter.h"

using namespace sru;
using namespace std;

string RemoveMemAddr(const string& src){
  string ret = src;
  string::size_type start = string::npos;
  while((start = ret.find("basic_object(0x")) != string::npos){
    string::size_type end = ret.find(")", start);
    assert(end != string::npos);
    ret = ret.substr(0, start) + "basic_object" + ret.substr(end+1);
  }
  return ret;
}

class CodeTestCase : public sru_test::TestCase {
 public:
  CodeTestCase(const char* name,
               const char* src,
               const char* result) :
                 TestCase(name),
                 src(src),
                 result(result){}
  virtual void operator()(){
    clock_t start = clock();
    LOG_ALWAYS << "Src: " << src;
    BasicObjectPtr r = Interpreter::Instance()->Eval(src);
    assert(r.get());
    LOG_ALWAYS << "ExpectResult:" << result;
    LOG_ALWAYS << "Result      :" << RemoveMemAddr(r->Inspect(0));
    assert(RemoveMemAddr(r->Inspect(0)) == result);
    LOG_ALWAYS << "time : " << ((static_cast<double>(clock() - start)) / CLOCKS_PER_SEC);
  }
  const char* src;
  const char* result;

  CodeTestCase(const CodeTestCase& obj);
  CodeTestCase &operator=(const CodeTestCase&obj);
};


#define TEST_CODE(name, src, result) \
  CodeTestCase TEST_##name("Regression_" #name, src, result)

#define SRUTEST(name, src) \
    TEST_CODE(name, src, "<True ... >")

TEST_CODE(number,"1", "<Numeric(1)>");
TEST_CODE(number2,"123", "<Numeric(123)>");
TEST_CODE(number_tos,"123.toS()", "<String(\"123\")>");
TEST_CODE(real,"1.23", "<Numeric(1.23)>");
TEST_CODE(real2,"0.23", "<Numeric(0.23)>");
TEST_CODE(real3,"0.0", "<Numeric(0)>");
TEST_CODE(real_tos,"1.23.toS()", "<String(\"1.23\")>");
TEST_CODE(string,"\"a\"", "<String(\"a\")>");
TEST_CODE(string2,"\"abc\"", "<String(\"abc\")>");
TEST_CODE(string_tos,"\"abc\".toS()", "<String(\"abc\")>");
TEST_CODE(lambda,"{x}", "<Proc({x;})>");
TEST_CODE(lambda2,"{|a|x}", "<Proc({|a|x;})>");
TEST_CODE(lambda3,"{|a,b|x}", "<Proc({|a,b|x;})>");
TEST_CODE(lambda4,"{||x}", "<Proc({x;})>");
TEST_CODE(lambda5,"{a|x}", "<Proc({|a|x;})>");
TEST_CODE(lambda6,"{a,b|x}", "<Proc({|a,b|x;})>");
TEST_CODE(scope, "a = 3;{a}()", "<Numeric(3)>");
TEST_CODE(scope2, "a = 2;{local.a = 3;a}()", "<Numeric(3)>");
TEST_CODE(scope3, "a = 2;{local.a = 3;{a}}()()", "<Numeric(3)>");
TEST_CODE(nil, "nil", "<nil ... >");
TEST_CODE(klass_v, "Class", "<Class ... >");
TEST_CODE(call, "{3}()", "<Numeric(3)>");
TEST_CODE(call2, "{|x|x}(3)", "<Numeric(3)>");
TEST_CODE(nulllambda,"{}", "<Proc({})>");
TEST_CODE(nulllambdacall,"{}()", "<nil ... >");
TEST_CODE(inst_call, "3.plus(2)", "<Numeric(5)>");
TEST_CODE(inst_call2, "3.plus 2", "<Numeric(5)>");
TEST_CODE(inst_call_call, "3.invert().invert()", "<Numeric(3)>");
TEST_CODE(let,"x = 3", "<Numeric(3)>");
TEST_CODE(let_result, "x = 3; x", "<Numeric(3)>");
TEST_CODE(native_field, "x = 3; x.plus", "<Proc({ -- Native Code -- })>");
TEST_CODE(native_field2, "3.plus", "<Proc({ -- Native Code -- })>");
TEST_CODE(call3, "(3.plus)(3,3)", "<Numeric(6)>");
TEST_CODE(call4, "x = 3; (x.plus)(x,3)", "<Numeric(6)>");
TEST_CODE(let_func, "t={|x|(x.plus)(x,3)}",
    "<Proc({|x|(x).plus(x, (Numeric).parse(Numeric, \"3\"));})>");
TEST_CODE(call_func, "t={|x|(x.plus)(x,3)}; t(5)", "<Numeric(8)>");
TEST_CODE(call_func2, "t={|x|(x.plus)(x,3)}; t 5", "<Numeric(8)>");
TEST_CODE(let_func2, "plus={|x,y|x+y}",
    "<Proc({|x,y|{|$$|($$).plus($$, y);}(x);})>");
TEST_CODE(use_func2, "plus={|x,y|x+y}; plus(4,3)", "<Numeric(7)>");
TEST_CODE(use_func3, "plus={|x,y|x+y}; plus 4,3", "<Numeric(7)>");
TEST_CODE(use_func4, "plus={|x,y|x+y}; plus(plus(4,3),plus(1,2))",
    "<Numeric(10)>");
TEST_CODE(use_func5, "plus={|x,y|x+y}; plus plus(4,3),plus(1,2)",
    "<Numeric(10)>");
TEST_CODE(use_func6, "plus={|x,y|x+y}; plus(4,3).plus(plus(1,2))",
    "<Numeric(10)>");
TEST_CODE(use_func7, "plus={|x,y|x+y}; plus(4,3).plus plus(1,2)",
    "<Numeric(10)>");
TEST_CODE(true_false, "(true.ifTrueFalse)(true,{1},{2})", "<Numeric(1)>");
TEST_CODE(true_false2, "(false.ifTrueFalse)(true,{1},{2})", "<Numeric(2)>");
TEST_CODE(true_false3, "true.ifTrueFalse({1},{2})", "<Numeric(1)>");
TEST_CODE(true_false4, "false.ifTrueFalse({1},{2})", "<Numeric(2)>");
TEST_CODE(true_equal, "true == true", "<True ... >");
TEST_CODE(true_equal2, "true == false", "<False ... >");
TEST_CODE(true_notequal, "true != false", "<True ... >");
TEST_CODE(true_notequal2, "true != true", "<False ... >");
TEST_CODE(false_equal, "false == false", "<True ... >");
TEST_CODE(false_equal2, "false == true", "<False ... >");
TEST_CODE(false_notequal, "false != true", "<True ... >");
TEST_CODE(false_notequal2, "false != false", "<False ... >");
TEST_CODE(if_statement , "if ( 1 == 1 ) then 3 end", "<Numeric(3)>");
TEST_CODE(if_statement2, "if ( 1 == 2 ) then 3 end", "<nil ... >");
TEST_CODE(if_statement3, "if ( 1 == 1 ) then 3 else 4 end", "<Numeric(3)>");
TEST_CODE(if_statement4, "if ( 1 == 2 ) then 3 else 4 end", "<Numeric(4)>");
TEST_CODE(if_statement5, "if ( 1 == 2 ) then 3 elsif ( 1 == 1 ) then 4 end",
    "<Numeric(4)>");
TEST_CODE(if_statement6,
    "if ( 1 == 2 ) then 3 "
    "elsif ( 1 == 1 ) then 4 else 5 end",
    "<Numeric(4)>");
TEST_CODE(if_statement7,
    "if ( 1 == 2 ) then 3 "
    "elsif ( 1 == 6 ) then 4 else 5 end",
    "<Numeric(5)>");
TEST_CODE(if_statement8,
    "if ( 1 == 2 ) then 3 "
    "elsif ( 1 == 6 ) then 4 "
    "elsif ( 1 == 7 ) then 5 end",
    "<nil ... >");
TEST_CODE(if_statement9,
    "if ( 1 == 2 ) then 3 "
    "elsif ( 1 == 6 ) then 4 "
    "elsif ( 1 == 7 ) then 5 else 6 end",
    "<Numeric(6)>");
TEST_CODE(add, "1+4", "<Numeric(5)>");
TEST_CODE(sub, "1-4", "<Numeric(-3)>");
TEST_CODE(mul, "1*4", "<Numeric(4)>");
TEST_CODE(div, "5/2", "<Numeric(2)>");
TEST_CODE(rem, "5%2", "<Numeric(1)>");
TEST_CODE(rem2, "5.3%2", "<Numeric(1.3)>");
TEST_CODE(rem3, "-3%2", "<Numeric(-1)>");
TEST_CODE(ltlt, "2 << 4", "<Numeric(32)>");
TEST_CODE(gtgt, "8 >> 2", "<Numeric(2)>");
TEST_CODE(lt, "2 < 3", "<True ... >");
TEST_CODE(lteq, "3 <= 3", "<True ... >");
TEST_CODE(gt, "2 > 3", "<False ... >");
TEST_CODE(gteq, "2 >= 3", "<False ... >");
TEST_CODE(while, "a=1;while(a<1000) a = a + a end; a", "<Numeric(1024)>");
TEST_CODE(while_break, "while(true) break(3) end", "<Numeric(3)>");
TEST_CODE(while_next,
    "a = 0\n"
    "while(true)\n"
    "  if(a==1) then break(a) end\n"  // TODO: Use post if
    "  a = 1\n"
    "  next()\n"
    "  break(3)\n"
    "end",
    "<Numeric(1)>");
TEST_CODE(def, "def a(x) x * x end; a(10)", "<Numeric(100)>");
TEST_CODE(obj, "Object.new()", "<basic_object ... >");
TEST_CODE(obj2, "Object.new().class", "<Object ... >");
TEST_CODE(obj3, "Object.subclass(\"Hoge\").new()", "<basic_object ... >");
TEST_CODE(obj4, "Object.subclass(\"Hoge\").new().class", "<Hoge ... >");
TEST_CODE(klass, "class MyClass end", "<MyClass ... >");
TEST_CODE(klass2, "class MyClass end; MyClass", "<MyClass ... >");
TEST_CODE(klass3, "class MyClass end; MyClass.new()", "<basic_object ... >");
TEST_CODE(klass3_5, "class MyClass end; MyClass.new().class", "<MyClass ... >");
TEST_CODE(klass4, "class MyClass < Numeric end; MyClass", "<MyClass ... >");
TEST_CODE(klass5, "class MyClass end; class MyClass2 < MyClass end",
    "<MyClass2 ... >");
TEST_CODE(klass6, "class MyClass end; MyClass.superclass", "<Object ... >");
TEST_CODE(klass7, "class MyClass < Numeric end; MyClass.superclass",
    "<Numeric ... >");
TEST_CODE(klass8,
    "class MyClass end;class MyClass2 < MyClass end;MyClass2.superclass",
    "<MyClass ... >");
TEST_CODE(klass9, "class MyClass def hoge() fuga() end end; MyClass.new()",
    "<basic_object ... >");
TEST_CODE(klass10, "class MyClass def hoge() fuga() end end; MyClass.new().hoge",
    "<Proc({|self:return|fuga();})>");
// TODO: add test for || && == === !+ > < >= <= | & << >> ! ~
TEST_CODE(multi,"a=1; 2;\n 3; a", "<Numeric(1)>");
TEST_CODE(multi_if,"if(true) then 3;2 end", "<Numeric(2)>");
TEST_CODE(multi_if2,"if(true) then 3\n2 end", "<Numeric(2)>");
TEST_CODE(multi_if3,"if(true) \n 3\n2 end", "<Numeric(2)>");
TEST_CODE(multi_def,"def x() 1;2 end; x()", "<Numeric(2)>");
TEST_CODE(multi_while,"a=1;b=0;while(a<101)  b=b+a;a=a+1 end;b",
    "<Numeric(5050)>");
TEST_CODE(multi_while2,"while(true) 2;break(3) end", "<Numeric(3)>");
TEST_CODE(multi_closure,"{1;2;3}()", "<Numeric(3)>");
TEST_CODE(require_native,"requireNative(\"./lib/hello.so\").ifTrue({1})","<Numeric(1)>");
TEST_CODE(require_native_exec,"requireNative(\"./lib/hello.so\");hello2()","<String(\"Hello world\")>");
TEST_CODE(array,"Array","<Array ... >");
TEST_CODE(array_new,"Array.new()","<Array size=0 []>");
TEST_CODE(array_push,"Array.new() << 3","<Array size=1 [<Numeric(3)>]>");
TEST_CODE(array_get,"a = Array.new() << 1 << 2 << 3; a[1]", "<Numeric(2)>");
TEST_CODE(array_set,"a = Array.new() << 1 << 2 << 3; a[1]=5;a", "<Array size=3 [<Numeric(1)>, <Numeric(5)>, <Numeric(3)>]>");
TEST_CODE(array_replace,"(Array.new() << 1 << 2 ).replace( Array.new() << 3 << 2 << 1)", "<Array size=3 [<Numeric(3)>, <Numeric(2)>, <Numeric(1)>]>");
TEST_CODE(array_reverse,"(Array.new() << 1 << 2 << 3).reverse()", "<Array size=3 [<Numeric(3)>, <Numeric(2)>, <Numeric(1)>]>");
TEST_CODE(array_reverse2,"(a = Array.new() << 1 << 2 << 3).reverse();a", "<Array size=3 [<Numeric(1)>, <Numeric(2)>, <Numeric(3)>]>");
TEST_CODE(array_reverse_ex,"(a = Array.new() << 1 << 2 << 3).reverse!();a", "<Array size=3 [<Numeric(3)>, <Numeric(2)>, <Numeric(1)>]>");
SRUTEST(array_concat, "a=[1,2];a.concat([3,4]);a.size() == 4");
TEST_CODE(numeric_times,"a=Array.new();3.times({a<<1});a", "<Array size=3 [<Numeric(1)>, <Numeric(1)>, <Numeric(1)>]>");
TEST_CODE(array_clear,"a=Array.new()<<1<<2;a.clear();a", "<Array size=0 []>");
TEST_CODE(array_size,"a=Array.new()<<1<<2;a.size()", "<Numeric(2)>");
TEST_CODE(array_stretch,"a=Array.new()<<1<<2;a[10]=3;a.size()", "<Numeric(11)>");
TEST_CODE(boolean_ampamp,"true && true", "<True ... >");
TEST_CODE(boolean_ampamp2,"true && false", "<False ... >");
TEST_CODE(boolean_pipepipe,"true || false", "<True ... >");
TEST_CODE(boolean_pipepipe2,"false || true", "<True ... >");
TEST_CODE(expression_factor,"!true", "<False ... >");
TEST_CODE(expression_factor2,"!!true", "<True ... >");
TEST_CODE(expression_factor3,"!!!true", "<False ... >");
TEST_CODE(math_sin,"Math.sin(1)", "<Numeric(0.841471)>");
TEST_CODE(math_cos,"Math.cos(1)", "<Numeric(0.540302)>");

// This test is flaky
TEST_CODE(math_rand,"Math.srand(1);Math.rand() == Math.rand()", "<False ... >");

TEST_CODE(math_srand,"Math.srand(1);"
                     "a = Math.rand();"
                     "Math.srand(1);"
                     "a==Math.rand()", "<True ... >");
TEST_CODE(comment_line,"Math.sin(#this is comment\n1)", "<Numeric(0.841471)>");
TEST_CODE(array_const, "[1,2,3].size()", "<Numeric(3)>");
TEST_CODE(array_const1, "[1,2,3][0]", "<Numeric(1)>");
TEST_CODE(array_const2, "[1,2,3][1]", "<Numeric(2)>");
TEST_CODE(sys_clock, "a=Sys.clock();b=Sys.clock();a<b", "<True ... >");
TEST_CODE(after_if, "a=1;a = 2 if true;a", "<Numeric(2)>");
TEST_CODE(after_if2, "a=1;a = 2 if false;a", "<Numeric(1)>");
TEST_CODE(spc, "def a()\n1 \n2\n1\nend;a()", "<Numeric(1)>");
TEST_CODE(proc_apply, "a={|a|a}; a.apply([1])", "<Numeric(1)>");
TEST_CODE(proc_apply2, "a={|a,b|b}; a.apply([1,2])", "<Numeric(2)>");
TEST_CODE(proc_ylppa, "a={|a|a}; a.ylppa()(1,2)",
    "<Array size=2 [<Numeric(1)>, <Numeric(2)>]>");
TEST_CODE(inheritance, "class X def a() 1 end end; class Y < X end; Y.new().a()", "<Numeric(1)>");
TEST_CODE(constructor, "class X def initialize() .hoge=1 end end; X.new().hoge", "<Numeric(1)>");
TEST_CODE(object_equal, "a=Object.new();b=Object.new();a==b", "<False ... >");
TEST_CODE(object_equal2, "a=Object.new();a==a", "<True ... >");
TEST_CODE(object_not_equal, "a=Object.new();b=Object.new();a!=b", "<True ... >");
TEST_CODE(object_not_equal2, "a=Object.new();a!=a", "<False ... >");
TEST_CODE(nil_equal, "nil==nil", "<True ... >");
TEST_CODE(nil_not_equal, "nil!=nil", "<False ... >");
TEST_CODE(string_equal     , "\"abcdef\" == \"abcdef\"" , "<True ... >");
TEST_CODE(string_equal2    , "\"abcdef\" == \"abcdefg\"", "<False ... >");
TEST_CODE(string_not_equal , "\"abcdef\" != \"abcdef\"" , "<False ... >");
TEST_CODE(string_not_equal2, "\"abcdef\" != \"abcdefg\"", "<True ... >");
SRUTEST(string_get,"a = \"abcd\";a[1] == \"b\"");
SRUTEST(string_get2,"a = \"abcd\";a[10] == \"\"");
SRUTEST(string_lessthan,"\"a\" < \"b\"");
SRUTEST(string_lessthan2,"(\"b\" < \"a\") == false");
SRUTEST(string_lessoreq,"\"a\" <= \"b\"");
SRUTEST(string_lessoreq2,"(\"a\" <= \"a\")");
SRUTEST(string_lessoreq3,"(\"b\" <= \"a\") == false");
SRUTEST(string_greatherthan,"(\"a\" > \"b\") == false");
SRUTEST(string_greatherthan2,"\"b\" > \"a\"");
SRUTEST(string_greatheroreq,"(\"a\" >= \"b\") == false");
SRUTEST(string_greatheroreq2,"\"a\" >= \"a\"");
SRUTEST(string_greatheroreq3,"\"b\" >= \"a\"");
SRUTEST(string_substr,"\"abcd\".substr(1,2) == \"bc\"");
SRUTEST(string_substr2,"\"abcd\".substr(1) == \"bcd\"");
SRUTEST(string_substr3,"\"abcd\".substr(1,20) == \"bcd\"");
SRUTEST(string_substr4,"\"abcd\".substr(20) == \"\"");
SRUTEST(string_substr5,"\"abcd\".substr(20,10) == \"\"");
SRUTEST(eval,"eval(__parser.parse(\"a = 3\").ast); a == 3");

/*
TEST_CODE(let_proc, "s = {|x|x;}", "<Proc({|x|x;})>");
TEST_CODE(let_proc_result, "s = {|x|x;}; s",
    "<Proc({|x|x;})>");
TEST_CODE(let_proc_result2, "s = {|x|x;}; s(4)",
    "<Numeric(4)>");
TEST_CODE(result_field, "s = {|x|x;}; s(4).plus",
    "<Proc({ -- Native Code -- })>");
*/
