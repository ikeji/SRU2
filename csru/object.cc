// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "object.h"

#include "basic_object.h"
#include "sru_string.h"
#include "constants.h"
#include "class.h"
#include "library.h"
#include "native_proc.h"
#include "utils.h"

using namespace sru;
using namespace std;

namespace {  // anonymous namespace for object class.

DEFINE_SRU_PROC(Equal){
  ARGLEN(2);
  return BooleanToObject(args[0].get() == args[1].get());
}

DEFINE_SRU_PROC(NotEqual){
  return BooleanToObject(args[0].get() != args[1].get());
}

} // anonymous namespace

namespace sru {

void InitializeObjectClass(const BasicObjectPtr& obj){
  Class::SetAsSubclass(obj, Library::Instance()->Nil());
  obj->Set(sym::__name(), SRUString::New(symbol("Object")));
  Class::SetAsInstanceMethod(obj,sym::equal(), CREATE_SRU_PROC(Equal));
  Class::SetAsInstanceMethod(obj,sym::notEqual(), CREATE_SRU_PROC(NotEqual));
}

}  // namespace sru
