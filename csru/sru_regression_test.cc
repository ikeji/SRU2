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
               const char* result):
                 TestCase(name),
                 src(src),
                 result(result){}
  virtual void operator()(){
    clock_t start = clock();
    LOG_ERROR << src;
    BasicObjectPtr r = Interpreter::Instance()->Eval(src);
    assert(r.get());
    LOG_ERROR << r->Inspect();
    assert(r->Inspect() == result);
    LOG_ALWAYS << "time : " << ((static_cast<double>(clock() - start)) / CLOCKS_PER_SEC);
  }
  const char* src;
  const char* result;
  CodeTestCase(const CodeTestCase& obj);
  CodeTestCase &operator=(const CodeTestCase&obj);

};


#define TEST_CODE(name, src, result) \
  CodeTestCase TEST_##name("Regression_" #name, src, result)

TEST_CODE(number,"1", "<Numeric: 1 >");

