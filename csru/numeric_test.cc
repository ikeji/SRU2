// Programing Language SRU
// Copyright(C) 2005-2009 IKeJI
// 

#include "testing.h"
#include <cassert>

#include "numeric.h"
#include "library.h"

using namespace sru;

TEST(SRUNumeric_Initialize){
  BasicObjectPtr obj = SRUNumeric::New(3);
  assert(obj->Get("class") == Library::Instance()->Numeric());
  assert(SRUNumeric::GetValue(obj) == 3);
  
  obj = SRUNumeric::New(4);
  assert(obj->Get("class") == Library::Instance()->Numeric());
  assert(SRUNumeric::GetValue(obj) == 4);
}

