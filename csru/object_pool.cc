// Programing Language SRU
// Copyright(C) 2001-2008 IKeJI
// 
//
#include "object_pool.h"

#include <map>
#include <string>
#include <vector>
#include "basic_object.h"

namespace sru {
namespace allocator {

void ObjectPool::Register(BasicObject * obj){
  allocated_.push_back(obj);
}

void ObjectPool::GarbageCollect(){
 
  // Initialize 
  std::vector<BasicObject*> marking;
  for(std::vector<BasicObject*>::iterator it = allocated_.begin();
      it != allocated_.end();
      it++) {
    if((*it)->gc_counter() > 1){
      marking.push_back(*it);
    } else {
      (*it)->set_gc_counter(-1);
    }
  }

  // Mark
  while( !marking.empty() ){
    BasicObject * cur = marking.back();
    if( cur->gc_counter() < 0 )
      cur->set_gc_counter(0);
    for(std::map<string,BasicObject*>::const_iterator it = cur->fields().begin();
        it != cur->fields().end();
        it++){
      if(it->second->gc_counter() < 0)
        marking.push_back(it->second);
    }
    
    if( cur->data() != NULL )
      cur->data()->Mark();
  }

  // Sweep
  for(std::vector<BasicObject*>::iterator it = allocated_.begin();
      it != allocated_.end();
      it++) {
    if( (*it)->gc_counter() < 0 ){
      delete *it;
      it = allocated_.erase(it);
    }
  }
}

ObjectPool::ObjectPool(){
}

ObjectPool ObjectPool::pool_;

} // namespace allocator
} // namespace sru
