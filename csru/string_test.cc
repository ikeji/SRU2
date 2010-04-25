// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include <cassert>
#include "string.h"
#include "library.h"
#include "constants.h"
#include "logging.h"
#include "symbol.h"

using namespace sru;
using namespace std;

TEST(SRUString_InitialiseTest){
  BasicObjectPtr obj = SRUString::New(symbol("hoge"));
  assert(obj->Get(sym::klass()).get() == Library::Instance()->String().get());
  assert(SRUString::GetValue(obj).to_str() == "hoge");
  LOG_ERROR << obj->Inspect();
  assert(obj->Inspect() == "<String(\"hoge\") findSlot:<Proc({ -- Native Code -- }) ... >>");
}

TEST(SRUString_ClassTest){
  LOG_ERROR << Library::Instance()->String()->Inspect();
  // TODO: Fix output format like <String>.
  assert(Library::Instance()->String()->Inspect() == "<String class:<Class class:..., findSlot:..., findSlotMethod:..., name:...>, name:<String(\"String\") ... >>");
}
