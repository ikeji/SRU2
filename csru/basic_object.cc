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
#include "sru_string.h"
#include "symbol.h"

using namespace sru;
using namespace std;

BasicObjectPtr BasicObject::New(){
  const BasicObjectPtr obj = new BasicObject();
  allocator::ObjectPool::Instance()->Register(obj.get());
  return obj;
}

BasicObjectPtr BasicObject::New(Value * data){
  const BasicObjectPtr obj = New();
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
  ostringstream s;

  string r;
  if(Data()) r = Data()->Inspect();
  if(!r.empty()){
    s << "<" << r;
  } else {
    string name = "";
    if(HasSlot(sym::__name()))
      name = SRUString::GetValue(Get(sym::__name())).to_str();
    if(name != ""){
      s << "<" << name;
    }else {
      s << "<basic_object(" << this << ")";
    }
  }
  int titlesize = (int)s.str().size() + 2;
  int sum = 0;
  vector<symbol> keys;
  for(fields_type::iterator it = fields.begin();
      it != fields.end();
      it++){
    symbol s = symbol::from_id(it->first);
    // Please add non useful fields here.
    if(!r.empty() && s == sym::klass()) continue;
    if(s == sym::superclass() ||
        s == sym::instanceMethods() ||
        s.to_str()[0] == '_')
      continue;
    sum += (int)s.to_str().size() + 5;
    keys.push_back(s);
  }
  if(keys.size() == 0) {
  } else if(limit < titlesize){
    s << " ... ";
  } else {
    sort(keys.begin(), keys.end());
    bool show_details = (sum < (limit - titlesize));
    int detailsize = (limit - titlesize - sum) / (fields.size()+1);
    if(detailsize < 0) detailsize = 0;
    for(vector<symbol>::iterator it = keys.begin();
        it != keys.end();
        it++){
      s << " ";
      if(show_details){
        s << it->to_str() << ":" << fields[it->getid()]->Inspect(detailsize);
      }else{
        s << it->to_str() << ":...";
      }
    }
  }
  s << ">";
  return s.str();
}

BasicObject::~BasicObject(){
  if (data) {
    data->Dispose();
  }
}

