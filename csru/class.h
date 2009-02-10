// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
// Class is a utility class for class system in SRU.
//


#ifndef CLASS_H_ 
#define CLASS_H_

#include <string>
#include "basic_object.h"

namespace sru {

class Class {
 public:
  static void InitializeInstance(BasicObjectPtr obj,BasicObjectPtr klass){
    obj->Set("class",klass);
    // TODO: InsertClassSystem
  }
};

} // namespace sru

#endif  // CLASS_H_

