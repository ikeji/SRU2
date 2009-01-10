// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "basic_object.h"

#include <string>
#include "object_pool.h"

using namespace sru;

BasicObject* BasicObject::New(){
  BasicObject* obj = new BasicObject();
  allocator::ObjectPool::Instance()->Register(obj);
  return obj;
}

void BasicObject::Mark(){
  allocator::ObjectPool::Instance()->Mark(this);
}
 
BasicObject::BasicObject(): gc_counter(0),data(NULL){
  fields.clear();
}

BasicObject::~BasicObject(){
  delete data;
}

BasicObjectPtr::BasicObjectPtr(BasicObject * p): ptr(p){
  if( ptr )
    ptr->IncrementGcCounter();
}

BasicObjectPtr::~BasicObjectPtr(){
  if( ptr )
    ptr->DecrementGcCounter();
}

void BasicObjectPtr::reset(BasicObject * p){
  if( ptr )
    ptr->DecrementGcCounter();
  ptr = p;
  if( ptr )
    ptr->IncrementGcCounter();
}

BasicObject& BasicObjectPtr::operator*() const{
  return *ptr;
}

BasicObject * BasicObjectPtr::operator->() const{
  return ptr;
}

BasicObject * BasicObjectPtr::get() const{
  return ptr;
}

void BasicObjectPtr::swap(BasicObjectPtr &b){
  std::swap(ptr, b.ptr);
}

BasicObjectPtr::BasicObjectPtr(const BasicObjectPtr& obj){
  ptr = obj.ptr;
  if( ptr )
    ptr->IncrementGcCounter();
}

BasicObjectPtr & BasicObjectPtr::operator=(const BasicObjectPtr &obj){
  if( this == &obj )
    return *this;

  if( ptr )
    ptr->DecrementGcCounter();
  ptr = obj.ptr;
  if( ptr )
    ptr->IncrementGcCounter();

  return *this;
}

