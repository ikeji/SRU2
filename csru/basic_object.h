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
#ifdef USEOLDGCC
#include <ext/hash_map>
#else
#include <tr1/unordered_map>
#endif
#include "logging.h"
#include "symbol.h"

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
  bool operator==(const BasicObjectPtr& obj) const {
    return (ptr == obj.ptr);
  }
  bool operator!=(const BasicObjectPtr& obj) const {
    return (ptr != obj.ptr);
  }
 private:
  BasicObject* ptr;
};

class BasicObject {
 public:
#ifdef USEOLDGCC
  typedef __gnu_cxx::hash_map<int,BasicObject* > fields_type;
#else
  typedef std::tr1::unordered_map<int,BasicObject* > fields_type;
#endif

  // Always use New method to allocate BasicObject instead of new operator.
  static BasicObjectPtr New();
  static BasicObjectPtr New(Value * value);

  // Setting or getting from this object's slot.
  void Set(const symbol& name,const BasicObjectPtr& ref){
#ifdef DEBUG_GC
    CHECK(!deleted) << "Why use deleted object?";
#endif
    CHECK(ref.get()) << "Don't set NULL";
    fields[name.getid()] = ref.get();
  }
  BasicObjectPtr Get(const symbol& name){
#ifdef DEBUG_GC
    CHECK(!deleted) << "Why use deleted object?";
#endif
    fields_type::iterator it = fields.find(name.getid());
    CHECK(it != fields.end()) << 
      "Error: unknwn slot: " << name.getid() << "(" << name.to_str() << ")" <<
      " in: " << Inspect();
    return it->second;
  };
  bool HasSlot(const symbol& name){
#ifdef DEBUG_GC
    CHECK(!deleted) << "Why use deleted object?";
#endif
    return (fields.find(name.getid()) != fields.end());
  }
  const fields_type& Fields() const{
#ifdef DEBUG_GC
    CHECK(!deleted) << "Why use deleted object?";
#endif
    return fields;
  }

  // Follow is GC related method.
  void Mark();
  int GcCounter() const { 
#ifdef DEBUG_GC
    CHECK(!deleted) << "Why use deleted object?";
#endif
    return gc_counter;
  }
  void SetGcCounter(int i) {
#ifdef DEBUG_GC
    CHECK(!deleted) << "Why use deleted object?";
#endif
    gc_counter = i; 
  }
  void IncrementGcCounter() {
#ifdef DEBUG_GC
    CHECK(!deleted) << "Why use deleted object?";
#endif
    gc_counter++; 
    LOG_TRACE <<"inc:" << Inspect() << " -> " << gc_counter;
  }
  void DecrementGcCounter() {
#ifdef DEBUG_GC
    CHECK(!deleted) << "Why use deleted object?";
#endif
    gc_counter--; 
#ifdef DEBUG_GC
    assert(gc_counter >= -1); 
#else
    assert(gc_counter >= 0); 
#endif
    LOG_TRACE <<"dec:" << Inspect() << " -> " << gc_counter;
  }

  Value* Data(){
#ifdef DEBUG_GC
    CHECK(!deleted) << "Why use deleted object?";
#endif
    return data; 
  }
  void SetData(Value* dat){
#ifdef DEBUG_GC
    CHECK(!deleted) << "Why use deleted object?";
#endif
    data = dat; 
  }

  template<typename T>
  T* GetData(){
#ifdef DEBUG_GC
    CHECK(!deleted) << "Why use deleted object?";
#endif
    T* r = dynamic_cast<T*>(data);
    return r;
  }

  std::string Inspect(int limit=80);

  ~BasicObject();
#ifdef DEBUG_GC
  bool deleted;
#endif
 private:
  BasicObject();
  
  fields_type fields;

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
  virtual void Dispose() = 0;
};

// NOTE: Mark as inline follow functions by performance issue.
inline BasicObjectPtr::BasicObjectPtr(BasicObject * p): ptr(p){
  if( ptr )
    ptr->IncrementGcCounter();
}

inline BasicObjectPtr::~BasicObjectPtr(){
  if( ptr )
    ptr->DecrementGcCounter();
}

inline void BasicObjectPtr::reset(BasicObject * p){
  if( ptr )
    ptr->DecrementGcCounter();
  ptr = p;
  if( ptr )
    ptr->IncrementGcCounter();
}

inline BasicObject& BasicObjectPtr::operator*() const{
  return *ptr;
}

inline BasicObject * BasicObjectPtr::operator->() const{
  return ptr;
}

inline BasicObject * BasicObjectPtr::get() const{
  return ptr;
}

inline void BasicObjectPtr::swap(BasicObjectPtr &b){
  std::swap(ptr, b.ptr);
}

inline BasicObjectPtr::BasicObjectPtr(const BasicObjectPtr& obj):
    ptr(obj.ptr){
  if( ptr )
    ptr->IncrementGcCounter();
}

inline BasicObjectPtr & BasicObjectPtr::operator=(const BasicObjectPtr &obj){
  if( this == &obj )
    return *this;

  if( ptr )
    ptr->DecrementGcCounter();
  ptr = obj.ptr;
  if( ptr )
    ptr->IncrementGcCounter();

  return *this;
}

} // namespace sru

#endif  // BASIC_OBJECT_H_
