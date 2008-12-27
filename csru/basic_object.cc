// Programing Language SRU
// Copyright(C) 2001-2008 IKeJI
// 
#include "basic_object.h"

#include <string>
#include "object_pool.h"

namespace sru {

BasicObject* BasicObject::New(){
  BasicObject* obj = new BasicObject();
  allocator::ObjectPool::Instance()->Register(obj);
  return obj;
}
 
BasicObject::BasicObject(){
  fields.clear();
  gc_counter = 0;
  data = NULL;
}

BasicObject::~BasicObject(){
  if(data != NULL)
    delete data;
}


BasicObjectPtr::BasicObjectPtr(BasicObject * p){
  ptr = p;
  if( ptr )
    ptr->InclimentGcCounter();
}

BasicObjectPtr::~BasicObjectPtr(){
  if( ptr )
    ptr->DeclimentGcCounter();
}

void BasicObjectPtr::reset(BasicObject * p){
  if( ptr )
    ptr->DeclimentGcCounter();
  ptr = p;
  if( ptr )
    ptr->InclimentGcCounter();
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
  BasicObject * work = ptr;
  ptr = b.ptr;
  b.ptr = work;
}

BasicObjectPtr::BasicObjectPtr(const BasicObjectPtr& obj){
  ptr = obj.ptr;
  if( ptr )
    ptr->InclimentGcCounter();
}

BasicObjectPtr & BasicObjectPtr::operator=(const BasicObjectPtr &obj){
  if( this == &obj )
    return *this;

  if( ptr )
    ptr->DeclimentGcCounter();
  ptr = obj.ptr;
  if( ptr )
    ptr->InclimentGcCounter();

  return *this;
}

void swap(BasicObjectPtr &a, BasicObjectPtr &b){
  a.swap(b);
}

} // namespace sru
