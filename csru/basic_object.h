// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
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

#ifndef BASIC_OBJECT_H_
#define BASIC_OBJECT_H_

#include <cassert>
#include <string>
#include <map>
using std::string;

namespace sru {

class BasicObject;

class Value {
 public:
  Value(){}
  virtual ~Value(){}
  virtual void Mark(){}
 private:
  Value(const Value& obj);
  Value &operator=(const Value& obj);
};

class BasicObject {
 public:
  static BasicObject* New();
  void Mark();
  void Set(const string& name,BasicObject *ref){
    fields.insert(std::make_pair(name,ref));
  }
  BasicObject* Get(const std::string& name){
    return fields[name];
  };
  const std::map<std::string,BasicObject *>& Fields() const{
    return fields; 
  }
  int GcCounter() const { return gc_counter; }
  void SetGcCounter(int i) { gc_counter = i; }
  void InclimentGcCounter() { gc_counter++; }
  void DeclimentGcCounter() {
    gc_counter--; 
    assert(gc_counter >= 0); 
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
  BasicObjectPtr &operator=(const BasicObjectPtr &obj);
 private:
  BasicObject* ptr;
};

void swap(BasicObjectPtr &a, BasicObjectPtr &b);

} // namespace sru

#endif  // BASIC_OBJECT_H_
