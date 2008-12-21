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
  static ObjectPool* instance(){ return &pool_; }
  void Register(BasicObject * obj);
  void GarbageCollect();
  int size(){ return allocated_.size(); }

 private:
  ObjectPool();

  static ObjectPool pool_;
  std::vector<BasicObject*> allocated_;

  ObjectPool(const ObjectPool& obj);
  ObjectPool &operator=(const BasicObject &obj);
};

} // namespace allocator
} // namespace sru
