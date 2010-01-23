// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
// Library class.
// Library class have many default classes in SRU.
// You can get pointer to these class from Library.
//

#ifndef LIBRARY_H_
#define LIBRARY_H_

#include "basic_object.h"

namespace sru {

class Library {
 public:
  static Library* Instance();

  // Nil is zen
  BasicObjectPtr Nil();

  // Class of Binding system
  BasicObjectPtr Binding();

  // Default Class System
  BasicObjectPtr Class();
  BasicObjectPtr Object();
  BasicObjectPtr Proc();

  // Default Classes
  BasicObjectPtr Array();
  BasicObjectPtr Hash();
  BasicObjectPtr String();
  BasicObjectPtr Numeric();
  BasicObjectPtr Boolean();
  BasicObjectPtr True();
  BasicObjectPtr False();
  BasicObjectPtr Parser();

  static void BindPrimitiveObjects(const BasicObjectPtr& obj);
 private:
  Library();
  ~Library();

  struct Impl;
  Impl *pimpl;

  Library(const Library& obj);
  Library &operator=(const Library& obj);
};

} // namespace sru

#endif  // LIBRARY_H_
