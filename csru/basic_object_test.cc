#include "testing.h"

#include <iostream>
#include <cassert>
#include "basic_object.h"

using namespace std;
using namespace sru_test;
using namespace sru;

TEST(BasicObject_CreateObjectTest){
  BasicObject *obj = BasicObject::New();
  assert(obj);
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

