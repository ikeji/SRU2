// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include <cassert>
#include <iostream>
#include "string.h"
#include "library.h"
#include "constants.h"

using namespace sru;
using namespace std;

TEST(SRUString_InitialiseTest){
  BasicObjectPtr obj = SRUString::New("hoge");
  assert(obj->Get(fCLASS).get() == Library::Instance()->String().get());
  assert(SRUString::GetValue(obj) == "hoge");
  assert(obj->Inspect() == "<\"hoge\">");
}

TEST(SRUString_ClassTest){
  cout << Library::Instance()->String()->Inspect() << endl;
  // TODO: Fix output format like <String>.
  assert(Library::Instance()->String()->Inspect() == "<String class:<Class class:..., findSlot:..., findSlotMethod:..., name:..., superclass:...>, name:<\"String\">, superclass:<Object>>");
}
