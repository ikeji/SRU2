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
  assert(boolean->Get(fCLASS) == Library::Instance()->Class());
  assert(boolean->Get(fSUPERCLASS) == Library::Instance()->Object());
}
TEST(Boolean_InitializeTrueTest){ 
  BasicObjectPtr tlue = Library::Instance()->True();
  assert(tlue.get());
  assert(tlue->Get(fCLASS) == Library::Instance()->Boolean());
  assert(tlue->HasSlot("ifTrue"));
  assert(tlue->HasSlot("ifFalse"));
  assert(tlue->HasSlot("ifTrueFalse"));
}

TEST(Boolean_InitializeFalseTest){ 
  BasicObjectPtr farse = Library::Instance()->False();
  assert(farse.get());
  assert(farse->Get(fCLASS) == Library::Instance()->Boolean());
  assert(farse->HasSlot("ifTrue"));
  assert(farse->HasSlot("ifFalse"));
  assert(farse->HasSlot("ifTrueFalse"));
}
