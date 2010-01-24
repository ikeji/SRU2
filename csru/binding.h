// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//

#ifndef BINDING_H_
#define BINDING_H_

#include <string>
#include "basic_object.h"
#include "library.h"
#include "constants.h"

namespace sru {

class Binding {
 public:
  static BasicObjectPtr New(const BasicObjectPtr& parent=NULL){
    BasicObjectPtr r = BasicObject::New();
    r->Set(fCLASS, Library::Instance()->Binding());
    if(parent.get() != NULL)
      r->Set(fPARENT_SCOPE, parent);
    return r;
  }
};

} // namespace sru

#endif  // BINDING_H_

