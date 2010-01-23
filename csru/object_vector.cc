// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "object_vector.h"
#include "basic_object.h"

namespace sru {

ptr_vector Conv(const object_vector& src){
  ptr_vector dst;
  for(object_vector::const_iterator it = src.begin();
      it != src.end();
      it++){
    dst.push_back(BasicObjectPtr(*it));
  }
  return dst;
}

object_vector Conv(const ptr_vector& src){
  object_vector dst;
  for(ptr_vector::const_iterator it = src.begin();
      it != src.end();
      it++){
    dst.push_back((*it).get());
  }
  return dst;
}

// TODO: Write test.
void MarkVector(object_vector* vec){
  for(object_vector::iterator it = vec->begin();
      it != vec->end();
      it++){
    (*it)->Mark();
  }
}

}  // namespace sru
