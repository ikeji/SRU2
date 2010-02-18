// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "object_pool.h"

#include <map>
#include <string>
#include <vector>
#include "basic_object.h"
#include "object_vector.h"

namespace sru {
namespace allocator {

struct ObjectPool::Impl {
  Impl():allocated() {}
  object_vector allocated;
};

ObjectPool::ObjectPool() : pimpl(new Impl){
}

ObjectPool::~ObjectPool(){
  delete pimpl;
}

void ObjectPool::Register(BasicObject * obj){
  pimpl->allocated.push_back(obj);
  // TODO: Define more good strategy.
#ifndef DEBUG_GC
  if(Size() > 1024*1024){
#else
  if(true){
#endif
    GarbageCollect();
  }
}

void ObjectPool::GarbageCollect(){
  // Initialize 
  object_vector root_object;
  for( object_vector::iterator it = pimpl->allocated.begin();
       it != pimpl->allocated.end();
       it++ ){
    if((*it)->GcCounter() > 0){
      root_object.push_back(*it);
    } else {
      (*it)->SetGcCounter(-1);
    }
  }

  // Mark
  for( object_vector::iterator it = root_object.begin();
       it != root_object.end();
       it++ ){
    Mark(*it);
  }

#ifdef DEBUG_GC
  for( object_vector::iterator it = pimpl->allocated.begin();
       it != pimpl->allocated.end();
      ){
    if( (*it)->GcCounter() < 0 ){
      LOG << "delete: " << (*it)->Inspect();
    }
    it++;
  }
#endif
  // Sweep
  for( object_vector::iterator it = pimpl->allocated.begin();
       it != pimpl->allocated.end();
      ){
    if( (*it)->GcCounter() < 0 ){
#ifdef DEBUG_GC
      (*it)->deleted = true;
#else
      delete *it;
#endif
      it = pimpl->allocated.erase(it);
    } else {
      it++;
    }
  }
}

void ObjectPool::Mark(BasicObject * obj){
  // Marking
  object_vector marking;
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

} // namespace allocator
} // namespace sru
