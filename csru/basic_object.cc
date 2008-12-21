// Programing Language SRU
// Copyright(C) 2001-2008 IKeJI
// 
#include "basic_object.h"

#include <string>
#include "object_pool.h"

namespace sru {

BasicObject* BasicObject::New(){
  BasicObject* obj = new BasicObject();
  allocator::ObjectPool::instance()->Register(obj);
  return obj;
}
 
BasicObject::BasicObject(){
  fields_.clear();
  gc_counter_ = 0;
  data_ = NULL;
}

BasicObject::~BasicObject(){
  if(data_ != NULL)
    delete data_;
}

} // namespace sru
