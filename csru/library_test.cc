// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include <cassert>
#include "library.h"

using namespace sru;

TEST(Library_GetPtrTest){
  assert(Library::Instance()->Nil().get());
  assert(Library::Instance()->Binding().get());
  assert(Library::Instance()->Class().get());
  assert(Library::Instance()->Object().get());
  assert(Library::Instance()->Proc().get());
  assert(Library::Instance()->Array().get());
  assert(Library::Instance()->Hash().get());
  assert(Library::Instance()->String().get());
  assert(Library::Instance()->Number().get());
  assert(Library::Instance()->Boolean().get());
  assert(Library::Instance()->True().get());
  assert(Library::Instance()->False().get());
}

