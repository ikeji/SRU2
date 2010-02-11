// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
//
// In SRU, all object is a instance of BasicObject.
// All class, nil, Integer and AST are instance of BasicObject.
// 
// BasicObject is controlled by GC.
// If you use BasicObject from C++, You have to hold by BasicObjectPtr.
// If you hold BasicObject by Data field, You have to implementation
// Mark method.
// You can't delete this object.
//

#ifndef BASIC_OBJECT_H_
#define BASIC_OBJECT_H_

#include <cassert>
#include <string>
#include <map>
#include "logging.h"

namespace sru {

class Value;
class BasicObject;

class BasicObjectPtr {
 public:
  BasicObjectPtr(BasicObject * p = NULL);
  ~BasicObjectPtr();

  void reset(BasicObject * p = NULL);
  BasicObject& operator*() const;
  BasicObject * operator->() const;
  BasicObject * get() const;

  void swap(BasicObjectPtr &b);

  BasicObjectPtr(const BasicObjectPtr& obj);
  BasicObjectPtr &operator=(const BasicObjectPtr& obj);
  bool operator==(const BasicObjectPtr& obj){
    return (ptr == obj.ptr);
  }
  bool operator!=(const BasicObjectPtr& obj){
    return (ptr != obj.ptr);
  }
 private:
  BasicObject* ptr;
};

class BasicObject {
 public:
  // Always use New method to allocate BasicObject instead of new operator.
  static BasicObjectPtr New();
  static BasicObjectPtr New(Value * value);

  // Setting or getting from this object's slot.
  void Set(const std::string& name,BasicObjectPtr ref){
    fields[name] = ref.get();
  }
  BasicObjectPtr Get(const std::string& name){
    CHECK(HasSlot(name)) << "Error: unknwn slot: " << name << " in: " << Inspect();
    assert(fields.find(name) != fields.end());
    return BasicObjectPtr(fields[name]);
  };
  bool HasSlot(const std::string& name){
    return (fields.find(name) != fields.end());
  }
  const std::map<std::string,BasicObject *>& Fields() const{
    return fields;
  }

  // Follow is GC related method.
  void Mark();
  int GcCounter() const { return gc_counter; }
  void SetGcCounter(int i) { gc_counter = i; }
  void IncrementGcCounter() {
    gc_counter++; 
    LOG_TRACE <<"inc:" << Inspect() << " -> " << gc_counter;
  }
  void DecrementGcCounter() {
    gc_counter--; 
    assert(gc_counter >= 0); 
    LOG_TRACE <<"dec:" << Inspect() << " -> " << gc_counter;
  }

  Value* Data(){ return data; }
  void SetData(Value* dat){ data = dat; }

  template<typename T>
  T* GetData(){
    T* r = dynamic_cast<T*>(data);
    CHECK(r) << "Can't cast " << Inspect() << " to real type " << T::name();
    assert(r);
    return r;
  }

  std::string Inspect(int limit=80);

  ~BasicObject();
 private:
  BasicObject();
  
  std::map<std::string,BasicObject* > fields;
  int gc_counter;
  Value* data;

  BasicObject(const BasicObject& obj);
  BasicObject &operator=(const BasicObject &obj);
};

class Value {
 public:
  Value(){}
  virtual ~Value(){}
  virtual void Mark(){}
  virtual std::string Inspect(){ return ""; }
};


} // namespace sru

#endif  // BASIC_OBJECT_H_
