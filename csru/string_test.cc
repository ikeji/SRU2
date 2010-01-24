// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include <cassert>
#include "string.h"
#include "library.h"
#include "constants.h"

using namespace sru;

TEST(SRUString_Initialise){
  BasicObjectPtr obj = SRUString::New("hoge");
  assert(obj->Get(fCLASS).get() == Library::Instance()->String().get());
  assert(SRUString::GetValue(obj) == "hoge");
  assert(obj->Inspect() == "<\"hoge\">");
}

