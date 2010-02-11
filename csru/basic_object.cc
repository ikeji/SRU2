// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "basic_object.h"

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include "object_pool.h"
#include "constants.h"
#include "string.h"

using namespace sru;
using namespace std;

BasicObjectPtr BasicObject::New(){
  BasicObjectPtr obj = new BasicObject();
  allocator::ObjectPool::Instance()->Register(obj.get());
  return obj;
}

BasicObjectPtr BasicObject::New(Value * data){
  BasicObjectPtr obj = New();
  obj->SetData(data);
  return obj;
}

void BasicObject::Mark(){
  allocator::ObjectPool::Instance()->Mark(this);
}
 
BasicObject::BasicObject(): fields(),gc_counter(0),data(NULL){
  fields.clear();
}

string BasicObject::Inspect(int limit){
  string r;
  if(Data()) r = Data()->Inspect();
  if(r.empty()){
    string name = "";
    if(HasSlot(fNAME))
      name = SRUString::GetValue(Get(fNAME));
    ostringstream s;
    if(name != ""){
      s << "<" << name;
    }else {
      s << "<basic_object(" << this << ")";
    }
    int titlesize = (int)s.str().size() + 2;
    if(limit > titlesize){
      s << " ";
      int sum = 0;
      for(map<string,BasicObject*>::iterator it = fields.begin();
          it != fields.end();
          it++){
        sum += (int)it->first.size() + 5;
      }
      bool show_details = (sum < (limit - titlesize));
      int detailsize = (limit - titlesize - sum) / (fields.size()+1);
      if(detailsize < 0) detailsize = 0;
      for(map<string,BasicObject*>::iterator it = fields.begin();
          it != fields.end();
          it++){
        if(it != fields.begin()) s << ", ";
        if(show_details){
          s << it->first << ":" << it->second->Inspect(detailsize);
        }else{
          s << it->first << ":...";
        }
      }
    }
    s << ">";
    r = s.str();
  }
  return r;
}

BasicObject::~BasicObject(){
  delete data;
}

BasicObjectPtr::BasicObjectPtr(BasicObject * p): ptr(p){
  if( ptr )
    ptr->IncrementGcCounter();
}

BasicObjectPtr::~BasicObjectPtr(){
  if( ptr )
    ptr->DecrementGcCounter();
}

void BasicObjectPtr::reset(BasicObject * p){
  if( ptr )
    ptr->DecrementGcCounter();
  ptr = p;
  if( ptr )
    ptr->IncrementGcCounter();
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
  std::swap(ptr, b.ptr);
}

BasicObjectPtr::BasicObjectPtr(const BasicObjectPtr& obj):
    ptr(obj.ptr){
  if( ptr )
    ptr->IncrementGcCounter();
}

BasicObjectPtr & BasicObjectPtr::operator=(const BasicObjectPtr &obj){
  if( this == &obj )
    return *this;

  if( ptr )
    ptr->DecrementGcCounter();
  ptr = obj.ptr;
  if( ptr )
    ptr->IncrementGcCounter();

  return *this;
}

