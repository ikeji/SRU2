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

} // namespace sru
