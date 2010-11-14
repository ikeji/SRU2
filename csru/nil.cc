// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#include "nil.h"

#include "basic_object.h"
#include "constants.h"
#include "sru_string.h"
#include "class.h"
#include "library.h"

namespace sru {

void SetupNilObject(const BasicObjectPtr& nil){
  Class::InitializeInstance(nil, Library::Instance()->Object());
  nil->Set(sym::__name(), SRUString::New(sym::nil()));
}

} // namespace sru
