// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "object_pool.h"

#include <ctime>
#include <map>
#include <string>
#include <vector>
#include "basic_object.h"
#include "object_container.h"
#include "logging.h"

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
  if(Size() > 1024*10){
#else
  if(true){
#endif
    GarbageCollect();
  }
}

void ObjectPool::GarbageCollect(){
#ifdef DEBUG_TRACE
  clock_t start = clock();
  LOG_ERROR << "Start GC";
#endif
  // Initialize 
  object_vector root_object;
  for( object_vector::iterator it = pimpl->allocated.begin();
       it != pimpl->allocated.end();
       it++ ){
    CHECK(*it) << "Why allcated has NULL?";
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
    int backup_gc_counter = (*it)->GcCounter();
    (*it)->SetGcCounter(-1);
    Mark(*it);
    (*it)->SetGcCounter(backup_gc_counter);
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
#ifdef DEBUG_TRACE
  int deleted = 0;
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
#ifdef DEBUG_TRACE
      deleted++;
#endif
    } else {
      it++;
    }
  }
#ifdef DEBUG_TRACE
  LOG_ERROR << "GC delete " << deleted << "/" <<
    (deleted + Size()) << " objects in " << 
    ((static_cast<double>(clock() - start)) / CLOCKS_PER_SEC) << " secs.";
#endif
}

void ObjectPool::Mark(BasicObject * obj){
  // Marking
  object_vector marking;
  CHECK(obj) << "Why mark call with NULL?";
  marking.push_back(obj);

  while( !marking.empty() ){
    BasicObject * cur = marking.back();
    marking.pop_back();
    CHECK(cur) << "Why mark NULL?";

    if( cur->GcCounter() < 0 ){
      cur->SetGcCounter(0);

      for(std::map<std::string,BasicObject*>::const_iterator it
          = cur->Fields().begin();
          it != cur->Fields().end();
          it++){
        CHECK(it->second) << "Why object has NULL?";
        if(it->second->GcCounter() < 0)
          marking.push_back(it->second);
      }

      if( cur->Data() != NULL )
        cur->Data()->Mark();
    }
  }
}

int ObjectPool::Size(){
  return pimpl->allocated.size(); 
}

} // namespace allocator
} // namespace sru
