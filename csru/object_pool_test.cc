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
  }
  void Mark(){
    MyObject->Mark();
  }
  BasicObject * MyObject;
};


int main(){
  assert(ObjectPool::Instance()->Size() == 0);

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
  assert(ObjectPool::Instance()->Size() == 2);
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 0);

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

  // Value Test
  BasicObjectPtr pv = BasicObject::New();
  pv->SetData(new MyValue());
  assert(ObjectPool::Instance()->Size() == 2);
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 2);
  // Multiple reference
  BasicObjectPtr pv2 = BasicObject::New();
  pv2->SetData(new MyValue());
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 4);
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 4);
  ((MyValue *)pv2->Data())->MyObject = ((MyValue *)pv->Data())->MyObject;
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 3);
  pv.reset();
  pv2.reset();
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 0);
  // Ring reference
  BasicObjectPtr pr = BasicObject::New();
  pr->SetData(new MyValue());
  BasicObjectPtr pr2 = BasicObject::New();
  pr2->SetData(new MyValue());
  ((MyValue *)pr2->Data())->MyObject = pr.get();
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 3);

  cout<< "OK" << endl;
}
