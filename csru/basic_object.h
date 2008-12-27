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

#include <cassert>
#include <string>
#include <map>
using std::string;

namespace sru {

namespace allocator {
class ObjectPool;
}

class Value {
 public:
  virtual void Mark() = 0;
  virtual ~Value() = 0;
 private:
  Value(const Value& obj);
  Value &operator=(const Value& obj);
};

class BasicObject {
 public:
  static BasicObject* New();
  void Set(const string& name,BasicObject *ref);
  BasicObject* Get(const string& name) const;
  const std::map<std::string,BasicObject *>& Fields() const{
    return fields; 
  }
  int GcCounter() const { return gc_counter; }
  void SetGcCounter(int i) { gc_counter = i; }
  void InclientGcCounter() { gc_counter++; }
  void DeclimentGcCounter() {
    gc_counter--; 
    assert(gc_counter > 0); 
  }
  Value* Data(){ return data; }
  void SetData(Value* dat){ data = dat; }
  ~BasicObject();

 private:
  BasicObject();
  
  std::map<std::string,BasicObject* > fields;
  int gc_counter;
  Value* data;

  BasicObject(const BasicObject& obj);
  BasicObject &operator=(const BasicObject &obj);
};

} // namespace sru