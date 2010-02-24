// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include <cassert>
#include "string.h"
#include "library.h"
#include "constants.h"
#include "logging.h"

using namespace sru;
using namespace std;

TEST(SRUString_InitialiseTest){
  BasicObjectPtr obj = SRUString::New("hoge");
  assert(obj->Get(sym::klass()).get() == Library::Instance()->String().get());
  assert(SRUString::GetValue(obj) == "hoge");
  assert(obj->Inspect() == "<\"hoge\">");
}

TEST(SRUString_ClassTest){
  LOG << Library::Instance()->String()->Inspect();
  // TODO: Fix output format like <String>.
  assert(Library::Instance()->String()->Inspect() == "<String class:<Class class:..., findSlot:..., findSlotMethod:..., name:..., superclass:...>, name:<\"String\">, superclass:<Object>>");
}
