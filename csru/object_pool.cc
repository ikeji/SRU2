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
  allocated.push_back(obj);
}

void ObjectPool::GarbageCollect(){
 
  // Initialize 
  std::vector<BasicObject*> marking;
  for(std::vector<BasicObject*>::iterator it = allocated.begin();
      it != allocated.end();
      it++) {
    if((*it)->GcCounter() > 0){
      marking.push_back(*it);
    } else {
      (*it)->SetGcCounter(-1);
    }
  }

  // Mark
  while( !marking.empty() ){
    BasicObject * cur = marking.back();
    marking.pop_back();
    if( cur->GcCounter() < 0 )
      cur->SetGcCounter(0);
    for(std::map<string,BasicObject*>::const_iterator it = cur->Fields().begin();
        it != cur->Fields().end();
        it++){
      if(it->second->GcCounter() < 0)
        marking.push_back(it->second);
    }
    
    if( cur->Data() != NULL )
      cur->Data()->Mark();
  }

  // Sweep
  std::vector<BasicObject*>::iterator it = allocated.begin();
  while( it != allocated.end() ){
    if( (*it)->GcCounter() < 0 ){
      delete *it;
      it = allocated.erase(it);
    } else {
      it++;
    }
  }
}

ObjectPool::ObjectPool(){
}

ObjectPool ObjectPool::pool;

} // namespace allocator
} // namespace sru
