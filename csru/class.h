// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
// Class is a utility class for class system in SRU.
//


#ifndef CLASS_H_ 
#define CLASS_H_

namespace sru {

class BasicObjectPtr;

class Class {
 public:
  static void InitializeInstance(BasicObjectPtr obj,BasicObjectPtr klass);
  static void SetAsSubclass(BasicObjectPtr obj,BasicObjectPtr klass);
  static void InitializeClassClass(BasicObjectPtr klass);
};

} // namespace sru

#endif  // CLASS_H_

