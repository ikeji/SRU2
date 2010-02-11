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
