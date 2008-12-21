// Programing Language SRU
// Copyright(C) 2001-2008 IKeJI
// 
// In SRU, all object is a instance of BasicObect.
// All class, nil, Integer and AST are instance of BasicObject.
// 
// BasicObject is controlled by GC.
// If you use BasicObject from C++, You have to hold by BasicObjecyRef.
// If you hold BasicObject by Data field, You have to implementation
// Mark method.
// You can't delete this object.
//

#include <string>
#include <map>
using std::string;

namespace sru {

namespace allocator {
class ObjectPool;
}
class Data {
 public:
  virtual void Mark() = 0;
  virtual ~Data() = 0;
 private:
  Data(const Data& obj);
  Data &operator=(const Data& obj);
};

class BasicObject {
 public:
  static BasicObject* New();
  void set(const string& name,BasicObject *ref);
  BasicObject* get(const string& name) const;
  const std::map<std::string,BasicObject *>& fields() const { return fields(); };
  int gc_counter() const { return gc_counter_; }
  void set_gc_counter(int i) { gc_counter_ = i; }
  Data * data(){ return data(); }
  void set_data(Data * data){ data_ = data; }
  virtual ~BasicObject();

 private:
  BasicObject();
  
  std::map<std::string,BasicObject* > fields_;
  int gc_counter_;
  Data* data_;

  BasicObject(const BasicObject& obj);
  BasicObject &operator=(const BasicObject &obj);
};


} // namespace sru
