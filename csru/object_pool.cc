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

using namespace std;

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
#ifdef DEBUG_TRACE
  clock_t start = clock();
  LOG_ERROR << "Start GC";
#endif

  // Mark
  for( object_vector::iterator it = pimpl->allocated.begin();
       it != pimpl->allocated.end();
       it++ ){
    if((*it)->GcCounter() > 0)
      Mark(*it, true);
  }

#ifdef DEBUG_GC
  for( object_vector::iterator it = pimpl->allocated.begin();
       it != pimpl->allocated.end();
      ){
    if( (*it)->GcCounter() == 0 ){
      LOG << "delete: " << (*it)->Inspect();
    }
    it++;
  }
#endif
#ifdef DEBUG_TRACE
  int deleted = 0;
#endif
  // Sweep
  object_vector::iterator to = pimpl->allocated.begin();
  for( object_vector::iterator from = pimpl->allocated.begin();
       from != pimpl->allocated.end();
       from++){
    if( (*from)->GcCounter() == 0 ){
#ifdef DEBUG_GC
      (*from)->deleted = true;
#else
      delete *from;
#endif
#ifdef DEBUG_TRACE
      deleted++;
#endif
    } else {
      if(to != from)
        *to = *from;
      if((*to)->GcCounter() == -1)
        (*to)->SetGcCounter(0);
      to++;
    }
  }
  pimpl->allocated.resize(distance(pimpl->allocated.begin(), to));
#ifdef DEBUG_TRACE
  LOG << "GC delete " << deleted << "/" <<
    (deleted + Size()) << " objects in " << 
    ((static_cast<double>(clock() - start)) / CLOCKS_PER_SEC) << " secs.";
#endif
}

typedef map<string, BasicObject*>::const_iterator fielditr;

void ObjectPool::Mark(BasicObject * obj, bool force){
  CHECK(obj) << "Why mark call with NULL?";
  // Already marked.
  if(!force && obj->GcCounter() != 0)
    return;

  if(obj->GcCounter() == 0)
    obj->SetGcCounter(-1);

  vector<pair<fielditr, fielditr> > marking_stack;
  if(!obj->Fields().empty())
    marking_stack.push_back(make_pair(obj->Fields().begin(), obj->Fields().end()));

  while( !marking_stack.empty() ){
    pair<fielditr, fielditr> pos = marking_stack.back();
    marking_stack.pop_back();
    while(pos.first != pos.second){
      BasicObject * cur = pos.first->second;
      pos.first++;

      if(cur->GcCounter() != 0)
        continue;

      cur->SetGcCounter(-1);

      if(cur->Data())
        cur->Data()->Mark();
      
      if(cur->Fields().empty())
        continue;

      marking_stack.push_back(pos);
      marking_stack.push_back(make_pair(cur->Fields().begin(), cur->Fields().end()));
      break;
    }
  }

  if(obj->Data())
    obj->Data()->Mark();
}

int ObjectPool::Size(){
  return pimpl->allocated.size(); 
}

} // namespace allocator
} // namespace sru
