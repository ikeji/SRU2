// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//

#ifndef BINDING_H_
#define BINDING_H_

#include "basic_object.h"  // for default argument.

namespace sru {

class Binding {
 public:
  static void InitializeClassObject(BasicObjectPtr numeric);
  static BasicObjectPtr New(const BasicObjectPtr& parent=NULL);
};

} // namespace sru

#endif  // BINDING_H_

