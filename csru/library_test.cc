// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include <cassert>
#include "library.h"

using namespace sru;

TEST(Library_GetPtrTest){
  assert(Library::Instance()->Nil());
  assert(Library::Instance()->Frame());
  assert(Library::Instance()->Class());
  assert(Library::Instance()->Object());
  assert(Library::Instance()->Proc());
  assert(Library::Instance()->Array());
  assert(Library::Instance()->Hash());
  assert(Library::Instance()->String());
  assert(Library::Instance()->Number());
  assert(Library::Instance()->Boolean());
  assert(Library::Instance()->True());
  assert(Library::Instance()->False());
}

