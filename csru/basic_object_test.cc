// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include "basic_object.h"

#include <iostream>
#include <cassert>

using namespace std;
using namespace sru_test;
using namespace sru;

TEST(BasicObject_CreateObjectTest){
  BasicObjectPtr obj = BasicObject::New();
  assert(obj.get());
}

TEST(BasicObject_WithoutValueTest){
  BasicObjectPtr obj = BasicObject::New();
  assert(obj.get());
  assert(obj->Data() == NULL);
}

TEST(BasicObject_WithValueTest){
  BasicObjectPtr obj = BasicObject::New(new Value());
  assert(obj.get());
  assert(obj->Data());
}

TEST(BasicObject_ObjectPtrTest){
  BasicObjectPtr p = BasicObject::New();
  BasicObjectPtr p2 = p.get();
  BasicObjectPtr p3;
  p3 = p2;
  assert( p3.get() == p2.get() );

  BasicObjectPtr p4;
  assert( p4.get() == NULL );
}

