#include <iostream>
#include <cassert>
#include "basic_object.h"
#include "object_pool.h"

using namespace sru;
using namespace std;
using namespace sru::allocator;

int main(){
  assert(ObjectPool::Instance()->Size() == 0);

  // GC test
  BasicObject * obj = BasicObject::New();
  BasicObject * obj2 = BasicObject::New();
  BasicObject * obj3 = BasicObject::New();
  assert(ObjectPool::Instance()->Size() == 3);
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

  cout<< "OK" << endl;
}
