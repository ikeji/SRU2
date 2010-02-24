// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "basic_object.h"

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
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
 
BasicObject::BasicObject():
#ifdef DEBUG_GC
  deleted(false),
#endif
  fields(),
  gc_counter(0),
  data(NULL){
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
      vector<string> keys;
      for(fields_type::iterator it = fields.begin();
          it != fields.end();
          it++){
        sum += (int)it->first.size() + 5;
        keys.push_back(it->first);
      }
      sort(keys.begin(), keys.end());
      bool show_details = (sum < (limit - titlesize));
      int detailsize = (limit - titlesize - sum) / (fields.size()+1);
      if(detailsize < 0) detailsize = 0;
      for(vector<string>::iterator it = keys.begin();
          it != keys.end();
          it++){
        if(it != keys.begin()) s << ", ";
        if(show_details){
          s << *it << ":" << fields[*it]->Inspect(detailsize);
        }else{
          s << *it << ":...";
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

