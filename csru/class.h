// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
// Class is a utility class for class system in SRU.
//


#ifndef CLASS_H_ 
#define CLASS_H_

#include <string>

namespace sru {

class BasicObjectPtr;

class Class {
 public:
  static void InitializeInstance(const BasicObjectPtr& obj,
                                 const BasicObjectPtr& klass);
  static void SetAsSubclass(const BasicObjectPtr& obj,
                            const BasicObjectPtr& super_klass);
  static void SetAsInstanceMethod(const BasicObjectPtr& klass,
                                  const std::string& name,
                                  const BasicObjectPtr& method);
  static void InitializeClassClass(const BasicObjectPtr& klass);
};

} // namespace sru

#endif  // CLASS_H_

