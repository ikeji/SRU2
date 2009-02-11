// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//

#ifndef BINDING_H_
#define BINDING_H_

#include <string>
#include "basic_object.h"
#include "library.h"

namespace sru {

const char* const PARENT_SCOPE = "parent";

class Binding {
 public:
  static BasicObjectPtr New(const BasicObjectPtr& parent=NULL){
    BasicObjectPtr r = BasicObject::New();
    r->Set("class", Library::Instance()->Binding());
    if(parent.get() != NULL)
      r->Set("parent", parent);
    return r;
  }
};

} // namespace sru

#endif  // BINDING_H_

