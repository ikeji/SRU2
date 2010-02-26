// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "object.h"

#include "basic_object.h"
#include "string.h"
#include "constants.h"
#include "class.h"
#include "library.h"

using namespace sru;
using namespace std;

namespace object_methods {
}  // namespace object_methods

namespace sru {

void InitializeObjectClass(const BasicObjectPtr& obj){
  Class::SetAsSubclass(obj, Library::Instance()->Nil());
  obj->Set(sym::name(), SRUString::New(symbol("Object")));
}

}  // namespace sru
