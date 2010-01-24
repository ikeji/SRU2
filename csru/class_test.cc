// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"

#include "class.h"

#include <cassert>
#include "basic_object.h"
#include "constants.h"

using namespace std;
using namespace sru;

TEST(Class_InitializeInstance){
  BasicObjectPtr klass = BasicObject::New();
  BasicObjectPtr instance = BasicObject::New();
  Class::InitializeInstance(instance, klass);
  assert(instance->Get(fCLASS) == klass);
}

