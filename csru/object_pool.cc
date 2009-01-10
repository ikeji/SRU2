// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "object_pool.h"

#include <map>
#include <string>
#include <vector>
#include "basic_object.h"

namespace sru {
namespace allocator {

struct ObjectPool::Impl {
  std::vector<BasicObject*> allocated;
};

void ObjectPool::Register(BasicObject * obj){
  pimpl->allocated.push_back(obj);
}

void ObjectPool::GarbageCollect(){
 
  // Initialize 
  std::vector<BasicObject*> root_object;
  for(std::vector<BasicObject*>::iterator it = pimpl->allocated.begin();
      it != pimpl->allocated.end();
      it++) {
    if((*it)->GcCounter() > 0){
      root_object.push_back(*it);
    } else {
      (*it)->SetGcCounter(-1);
    }
  }

  // Mark
  for( std::vector<BasicObject*>::iterator it = root_object.begin();
       it != root_object.end();
       it++
      ){
    Mark(*it);
  }

  // Sweep
  for(std::vector<BasicObject*>::iterator it = pimpl->allocated.begin();
      it != pimpl->allocated.end();
      ){
    if( (*it)->GcCounter() < 0 ){
      delete *it;
      it = pimpl->allocated.erase(it);
    } else {
      it++;
    }
  }
}

void ObjectPool::Mark(BasicObject * obj){
  // Marking
  std::vector<BasicObject*> marking;
  marking.push_back(obj);

  while( !marking.empty() ){
    BasicObject * cur = marking.back();
    marking.pop_back();

    if( cur->GcCounter() < 0 )
      cur->SetGcCounter(0);
    for(std::map<std::string,BasicObject*>::const_iterator it
            = cur->Fields().begin();
        it != cur->Fields().end();
        it++){
      if(it->second->GcCounter() < 0)
        marking.push_back(it->second);
    }

    if( cur->Data() != NULL )
      cur->Data()->Mark();
  }
}

int ObjectPool::Size(){
  return pimpl->allocated.size(); 
}

ObjectPool::ObjectPool() : pimpl(new Impl){
}

ObjectPool::~ObjectPool(){
  delete pimpl;
}

} // namespace allocator
} // namespace sru
