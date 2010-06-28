// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
// Memory management and GC.
// All object are regestered to ObjectPool.
//

#ifndef OBJECT_POOL_H_
#define OBJECT_POOL_H_

namespace sru {
class BasicObject;
}

namespace sru {
namespace allocator {

class ObjectPool{
 public:
  static ObjectPool* Instance(){
    static ObjectPool pool;
    return &pool; 
  }
  void Register(BasicObject * obj);
  void GarbageCollect();
  void Mark(BasicObject * obj, bool force = false);
  int Size();

 private:
  ObjectPool();
  ~ObjectPool();

  struct Impl;
  Impl* pimpl;

  ObjectPool(const ObjectPool& obj);
  ObjectPool &operator=(const ObjectPool& obj);
};

} // namespace allocator
} // namespace sru

#endif // OBJECT_POOL_H_
