// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include "object_container.h"

#include <cassert>
#include "basic_object.h"
#include "object_pool.h"

using namespace sru;
using namespace sru::allocator;
using namespace std;


TEST(ObjectVector_ConvTest){
  assert(ObjectPool::Instance()->Size() == 0);
  ptr_vector v1;
  v1.push_back(BasicObject::New());
  v1.push_back(BasicObject::New());
  v1.push_back(BasicObject::New());
  assert(ObjectPool::Instance()->Size() == 3);
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 3);
  object_vector v2 = Conv(v1);
  v1.clear();
  assert(ObjectPool::Instance()->Size() == 3);
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 0);
}

