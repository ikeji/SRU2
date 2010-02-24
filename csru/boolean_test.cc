// Programing Language SRU
// Copyright(C) 2005-2009 IKeJI
// 

#include "testing.h"
#include <cassert>

#include "library.h"
#include "basic_object.h"
#include "constants.h"

using namespace sru;
using namespace sru_test;

TEST(Boolean_InitializeTest){
  BasicObjectPtr boolean = Library::Instance()->Boolean();
  assert(boolean.get());
  assert(boolean->Get(sym::klass()) == Library::Instance()->Class());
  assert(boolean->Get(sym::superclass()) == Library::Instance()->Object());
}
TEST(Boolean_InitializeTrueTest){ 
  BasicObjectPtr tlue = Library::Instance()->True();
  assert(tlue.get());
  assert(tlue->Get(sym::klass()) == Library::Instance()->Boolean());
  assert(tlue->HasSlot(sym::ifTrue()));
  assert(tlue->HasSlot(sym::ifFalse()));
  assert(tlue->HasSlot(sym::ifTrueFalse()));
}

TEST(Boolean_InitializeFalseTest){ 
  BasicObjectPtr farse = Library::Instance()->False();
  assert(farse.get());
  assert(farse->Get(sym::klass()) == Library::Instance()->Boolean());
  assert(farse->HasSlot(sym::ifTrue()));
  assert(farse->HasSlot(sym::ifFalse()));
  assert(farse->HasSlot(sym::ifTrueFalse()));
}
