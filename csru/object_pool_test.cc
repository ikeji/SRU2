// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "testing.h"
#include <iostream>
#include <cassert>
#include "basic_object.h"
#include "object_pool.h"

using namespace sru;
using namespace std;
using namespace sru::allocator;

class MyValue : public Value{
 public:
  MyValue(){
    MyObject = BasicObject::New();
    Count++;
  }
  ~MyValue(){
    Count--;
  }
  void Mark(){
    MyObject->Mark();
  }
  BasicObject * MyObject;
  static int Count;
};

int MyValue::Count;

TEST(ObjectPool_SizeTest){
  assert(ObjectPool::Instance()->Size() == 0);
  assert(MyValue::Count == 0);
}

TEST(ObjectPool_GCTest){
  // GC test
  BasicObject * obj = BasicObject::New();
  BasicObject * obj2 = BasicObject::New();
  BasicObject * obj3 = BasicObject::New();
  assert(ObjectPool::Instance()->Size() == 3);
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 0);
  // ring test
  BasicObject * objr = BasicObject::New();
  BasicObject * objr2 = BasicObject::New();
  objr->Set(string("test"),objr2);
  objr2->Set(string("test"),objr);
  assert(ObjectPool::Instance()->Size() == 2);
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 0);
}

TEST(ObjectPool_RefTest){
  // RefTest
  BasicObjectPtr p = BasicObject::New();
  BasicObjectPtr p2 = BasicObject::New();
  BasicObjectPtr p3 = BasicObject::New();
  assert(ObjectPool::Instance()->Size() == 3);
  p2.reset();
  assert(ObjectPool::Instance()->Size() == 3);
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 2);
  p = p3; 
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 1);
  p.reset();
  p3.reset();
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 0);
}

TEST(ObjectPool_ValueTest){
  // Value Test
  BasicObjectPtr pv = BasicObject::New(new MyValue());
  assert(ObjectPool::Instance()->Size() == 2);
  assert(MyValue::Count == 1);
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 2);
  assert(MyValue::Count == 1);
  // Multiple reference
  BasicObjectPtr pv2 = BasicObject::New(new MyValue());
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 4);
  assert(MyValue::Count == 2);
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 4);
  assert(MyValue::Count == 2);
  ObjectPool::Instance()->GarbageCollect();
  dynamic_cast<MyValue *>(pv2->Data())->MyObject =
      dynamic_cast<MyValue *>(pv->Data())->MyObject;
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 3);
  assert(MyValue::Count == 2);
  ObjectPool::Instance()->GarbageCollect();
  pv.reset();
  pv2.reset();
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 0);
  assert(MyValue::Count == 0);
  // Ring reference
  BasicObjectPtr pr = BasicObject::New(new MyValue());
  BasicObjectPtr pr2 = BasicObject::New(new MyValue());
  dynamic_cast<MyValue *>(pr2->Data())->MyObject = pr.get();
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 3);
  assert(MyValue::Count == 2);
  pr.reset();
  pr2.reset();
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 0);
  assert(MyValue::Count == 0);
}

