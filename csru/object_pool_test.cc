#include <iostream>
#include <cassert>
#include "basic_object.h"
#include "object_pool.h"

using namespace sru;
using namespace std;
using namespace sru::allocator;
int main(){
  BasicObject * obj = BasicObject::New();
  BasicObject * obj2 = BasicObject::New();
  BasicObject * obj3 = BasicObject::New();
  assert(ObjectPool::Instance()->Size() == 3);
  ObjectPool::Instance()->GarbageCollect();
  assert(ObjectPool::Instance()->Size() == 0);
  // TODO: need test
  cout<< "OK" << endl;
}
