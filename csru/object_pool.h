// Programing Language SRU
// Copyright(C) 2001-2008 IKeJI
// 
//

#include <vector>
#include <string>

namespace sru {
class BasicObject;
}

namespace sru {
namespace allocator {

class ObjectPool{
 public:
  static ObjectPool* Instance(){ return &pool; }
  void Register(BasicObject * obj);
  void GarbageCollect();
  void Mark(BasicObject * obj);
  int Size(){ return allocated.size(); }

 private:
  ObjectPool();

  static ObjectPool pool;
  std::vector<BasicObject*> allocated;

  ObjectPool(const ObjectPool& obj);
  ObjectPool &operator=(const BasicObject &obj);
};

} // namespace allocator
} // namespace sru
