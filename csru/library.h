// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#ifndef LIBRARY_H_
#define LIBRARY_H_

#include "basic_object.h"

namespace sru {

class Library {
 public:
  static Library* Instance();

  // Nil is zen
  BasicObject* Nil();

  // Class of Frame system
  BasicObject* Frame();

  // Default Class System
  BasicObject* Class();
  BasicObject* Object();
  BasicObject* Proc();

  // Default Classes
  BasicObject* Array();
  BasicObject* Hash();
  BasicObject* String();
  BasicObject* Number();
  BasicObject* Boolean();
  BasicObject* True();
  BasicObject* False();

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
