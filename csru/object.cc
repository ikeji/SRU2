// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "object.h"

#include "basic_object.h"
#include "string.h"

using namespace sru;
using namespace std;

namespace object_methods {
}  // namespace object_methods

namespace sru {

void InitializeObjectClass(const BasicObjectPtr& obj){
  obj->Set("name", SRUString::New("Object"));
}

}  // namespace sru
